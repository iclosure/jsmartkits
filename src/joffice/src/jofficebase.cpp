#include "precomp.h"
#include "jofficebase.h"
#include <ActiveQt/QAxWidget>
#include <sapi.h>

#define J_AXBASE_GENERATE_DOCUMENT 0

// - class JOfficeBasePrivate -

class JOfficeBasePrivate
{
    J_DECLARE_PUBLIC(JOfficeBase)
    public:
        JOfficeBasePrivate(JOfficeBase::OfficeType officeType, JOfficeBase *parent) :
      q_ptr(parent),
        application(0),
        workbooks(0),
        workbook(0),
        officeType(officeType)
    {
        //
        HRESULT r = OleInitialize(0);
        if (r != S_OK && r != S_FALSE) {
            //qWarning(QStringLiteral("Qt:初始化Ole失败（error %x）"), (unsigned int)r);
        }

        application = new QAxObject("Excel.Application");

        switch (officeType) {
        case JOfficeBase::OfficeWord:
            application->setControl("Word.Application");
            break;
        case JOfficeBase::OfficeExcel:
            application = new QAxObject();
            application->setControl("Excel.Application");
            break;
        default:
            break;
        }


        //
        if (application->isNull()) {
            Q_ASSERT(application);
        } else {
            application->setProperty("WindowState", 2);
            application->dynamicCall("SetVisible(bool)", false);
            application->setProperty("DisplayAlerts", false);
            workbooks = application->querySubObject("Workbooks");
        }
#if J_AXBASE_GENERATE_DOCUMENT
        generateDocumentFile(application);
        generateDocumentFile(workbooks);
#endif
    }

    ~JOfficeBasePrivate()
    {
        Q_Q(JOfficeBase);
        q->close();

        delete application;
        application = 0;

        //
        ::OleUninitialize();
    }

    bool isValid() const;

    static bool generateDocumentFile(QAxBase *axBase, const QString &filePath = QString());

private:
    QAxObject *application;
    QAxObject *workbooks;
    QAxObject *workbook;
    JOfficeBase::OfficeType officeType;
};

bool JOfficeBasePrivate::isValid() const
{
    return (application && !application->isNull());
}

bool JOfficeBasePrivate::generateDocumentFile(QAxBase *axBase, const QString &filePath)
{
    //
    if (!axBase || axBase->isNull()) {
        return false;
    }

    //
    QString _filePath = filePath;
    if (_filePath.isEmpty()) {
        const QMetaObject *mobj = axBase->metaObject();
        _filePath = QApplication::applicationDirPath().append("/docs/help/active/");
        _filePath.append(mobj->classInfo(mobj->indexOfClassInfo("Interface " + QByteArray::number(1))).value());
        _filePath.append(QString("_%1.html").arg(axBase->control()));
    }

    //
    QFileInfo fileInfo(_filePath);
    QDir dir = fileInfo.absoluteDir();
    if (!dir.exists()) {
        if (!dir.mkpath(dir.path())) {
            return false;
        }
    }

    //
    QFile file(_filePath);
    if (!file.open(QFile::WriteOnly)) {
        return false;
    }

    //
    QTextStream stream(&file);
    stream << axBase->generateDocumentation() << endl;

    return true;
}

// - class JOfficeBase -

JOfficeBase::JOfficeBase(OfficeType officeType, QObject *parent) :
    QObject(parent),
    d_ptr(new JOfficeBasePrivate(officeType, this))
{

}

JOfficeBase::~JOfficeBase()
{
    Q_D(JOfficeBase);
    delete d;
}

bool JOfficeBase::isValid() const
{
    Q_D(const JOfficeBase);
    return d->isValid();
}

JOfficeBase::OfficeType JOfficeBase::officeType() const
{
    Q_D(const JOfficeBase);
    return d->officeType;
}

QAxObject *JOfficeBase::application()
{
    Q_D(JOfficeBase);
    return d->application;
}

QAxObject *JOfficeBase::workbooks()
{
    Q_D(JOfficeBase);
    return d->application->querySubObject("WorkBooks");
}

QAxObject *JOfficeBase::workbook(int index)
{
    QAxObject *workbooks = this->workbooks();
    if (workbooks == 0) {
        return 0;
    }

    return workbooks->querySubObject("Item(QVariant)", index);
}

QAxObject *JOfficeBase::activeWorkbook()
{
    Q_D(JOfficeBase);
    if (d->workbook) {
        return d->workbook;
    } else {
        return d->application->querySubObject("ActiveWorkBook");
    }
}

QAxObject *JOfficeBase::sheets()
{
    Q_D(JOfficeBase);
    QAxObject *workbook = d->workbook;
    if (workbook == 0) {
        workbook = activeWorkbook();
    }

    if (workbook == 0) {
        return 0;
    }

    return workbook->querySubObject("Sheets");
}

QAxObject *JOfficeBase::sheet(int index)
{
    QAxObject *sheets = this->sheets();
    if (sheets == 0) {
        return 0;
    }
#if J_AXBASE_GENERATE_DOCUMENT
    generateDocumentFile(sheets);
#endif

    return sheets->querySubObject("Item(QVariant)", index);
}

QAxObject *JOfficeBase::prependSheet(const QString &sheetName)
{
    return insertSheet(1, sheetName);
}

QAxObject *JOfficeBase::appendSheet(const QString &sheetName)
{
    QAxObject *sheets = this->sheets();
    if (sheets == 0) {
        return 0;
    }

    int sheetCount = sheets->property("Count").toInt();
    QAxObject *lastSheet = sheets->querySubObject("Item(QVariant)", sheetCount);
    if (lastSheet == 0) {
        return 0;
    }

    QAxObject *newSheet = sheets->querySubObject("Add(QVariant)", lastSheet->asVariant());
    if (newSheet == 0) {
        return 0;
    }

    lastSheet->dynamicCall("Move(QVariant)", newSheet->asVariant());
    if (!sheetName.isEmpty()) {
        newSheet->setProperty("Name", sheetName);
    }

    return newSheet;
}

QAxObject *JOfficeBase::insertSheet(int index, const QString &sheetName)
{
    QAxObject *sheets = this->sheets();
    if (sheets == 0) {
        return 0;
    }

    int sheetCount = sheets->property("Count").toInt();
    if (sheetCount <= index) {
        return appendSheet(sheetName);
    }

    QAxObject *beforeSheet = sheets->querySubObject("Item(QVariant)", index);
    if (beforeSheet == 0) {
        return 0;
    }

    QAxObject *newSheet = sheets->querySubObject("Add(QVariant)", beforeSheet->asVariant());
    if (newSheet == 0) {
        return 0;
    }

    if (!sheetName.isEmpty()) {
        newSheet->setProperty("Name", sheetName);
    }

    return newSheet;
}

bool JOfficeBase::setSheetCount(int count)
{
    QAxObject *sheets = this->sheets();
    if (sheets == 0) {
        return false;
    }

    int sheetCount = sheets->property("Count").toInt();
    if (sheetCount == count) {
        return true;
    }

    clearSheet();

    for (int i = 0; i < count - 1; ++i) {
        appendSheet("");
    }

    return true;
}

bool JOfficeBase::clearSheet()
{
    QAxObject *sheets = this->sheets();
    if (sheets == 0) {
        return false;
    }

    int sheetCount = sheets->property("Count").toInt();
    while (sheetCount > 1) {
        QAxObject *sheet = sheets->querySubObject("Item(QVariant)", sheetCount);
        if (sheet) {
            sheet->dynamicCall("Delete()");
        }
        sheetCount = sheets->property("Count").toInt();
    }

    return true;
}

bool JOfficeBase::create()
{
    Q_D(JOfficeBase);
    d->workbooks->dynamicCall("Add(QVariant)", QDir::toNativeSeparators(
                                  QApplication::applicationDirPath()
                                  .append("/../config/office/templates/workbook_template_1.xltx")));
    d->workbook = d->workbooks->querySubObject("Item(QVariant)",
                                               d->workbooks->property("Count"));
    if (d->workbook == 0) {
        return false;
    }
#if J_AXBASE_GENERATE_DOCUMENT
    generateDocumentFile(d->workbook);

    //
    QAxObject *sheet1 = sheet(1);
    if (sheet1) {
        generateDocumentFile(sheet1);
        QAxObject *range1 = sheet1->querySubObject("Range(QVariant)", "A1:B2");
        if (range1) {
            generateDocumentFile(range1);
            QAxObject *interior = range1->querySubObject("Interior");
            if (interior) {
                generateDocumentFile(interior);
            }
            QAxObject *cellFont = range1->querySubObject("Font");
            if (cellFont) {
                generateDocumentFile(cellFont);
            }
            QAxObject *borders = range1->querySubObject("Borders");
            if (borders) {
                generateDocumentFile(borders);
            }
        }
    }
#endif

    return true;
}

bool JOfficeBase::generateDocumentFile(QAxBase *axBase, const QString &filePath)
{
    return JOfficeBasePrivate::generateDocumentFile(axBase, filePath);
}

bool JOfficeBase::open(const QString &filePath, QIODevice::OpenMode mode)
{
    Q_D(JOfficeBase);
    QFileInfo fileInfo(filePath);
    if (fileInfo.exists()) {
        d->workbook = d->workbooks->querySubObject("Open(QVariant)",
                                                   QDir::toNativeSeparators(fileInfo.absoluteFilePath()));
    } else if ((mode != QIODevice::ReadOnly) && !create()) {
        return false;
    }

    if (d->workbook == 0) {
        return false;
    }

    return true;
}

bool JOfficeBase::saveAs(const QString &filePath)
{
    Q_D(JOfficeBase);
    if (d->workbook == 0) {
        return false;
    }

    d->workbook->dynamicCall("SaveAs(QVariant)", QDir::toNativeSeparators(filePath));

    return true;
}

bool JOfficeBase::close()
{
    Q_D(JOfficeBase);
    if (d->workbook) {
        d->workbook->dynamicCall("Close(bool)", false);
    }

    d->application->dynamicCall("Quit(void)");

    return true;
}

bool JOfficeBase::show(bool enabled)
{
    Q_D(JOfficeBase);
    if (d->application->isNull()) {
        return false;
    }

    d->application->setProperty("WindowState", 1);
    d->application->setProperty("Visible", enabled);
    d->application->setProperty("ShowWindowsInTaskbar", enabled);
    return true;
}
