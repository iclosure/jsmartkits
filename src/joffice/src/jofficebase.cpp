#include "precomp.h"
#include "jofficebase.h"
#include <ActiveQt/QAxWidget>

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
        application = new QAxObject();

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
            application->dynamicCall("SetVisible(bool)", false);
            application->setProperty("DisplayAlerts", false);
            workbooks = application->querySubObject("Workbooks");
        }
    }

    ~JOfficeBasePrivate()
    {
        Q_Q(JOfficeBase);
        q->quit();

        delete application;
        application = 0;
    }

    bool isValid() const;

    static bool generateDocumentFile(QAxBase *axBase, const QString &filePath);

private:
    JOfficeBase::OfficeType officeType;
    QAxObject *application;
    QAxObject *workbooks;
    QAxObject *workbook;
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
    return workbook->querySubObject("Sheets");
}

QAxObject *JOfficeBase::sheet(int index)
{
    QAxObject *sheets = this->sheets();
    if (sheets == 0) {
        return 0;
    }

    return sheets->querySubObject("Item(QVariant)", index);
}

bool JOfficeBase::generateDocumentFile(QAxBase *axBase, const QString &filePath)
{
    return JOfficeBasePrivate::generateDocumentFile(axBase, filePath);
}

bool JOfficeBase::open(const QString &filePath)
{
    Q_D(JOfficeBase);
    QString f = QApplication::applicationDirPath().append(QStringLiteral("/../docs/工作量预估表 - Copy.xlsx"));
    if (QFile(filePath).exists()) {
        d->workbook = d->workbooks->querySubObject("Open(QVariant)", filePath);
    } else {
        d->workbooks->dynamicCall("Add(QVariant)", f);
        d->workbook = d->workbooks->querySubObject("Item(QVariant)",
                                                   d->workbooks->property("Count"));
    }

    if (d->workbook == 0) {
        return false;
    }

    return true;
}

bool JOfficeBase::quit()
{
    Q_D(JOfficeBase);
    return d->application->dynamicCall("Quit()").toBool();
}
