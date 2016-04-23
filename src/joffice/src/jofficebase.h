#ifndef JOFFICEBASE_H
#define JOFFICEBASE_H

#include "joffice_global.h"
#include <QObject>
#include <QAxObject>

// - class JOfficeBase -

class JOfficeBasePrivate;

class JOFFICE_EXPORT JOfficeBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(OfficeType officeType READ officeType)
public:
    enum OfficeType {
        OfficeInvalid = -1,
        OfficeExcel,
        OfficeWord,
    };

    explicit JOfficeBase(OfficeType officeType, QObject *parent = 0);
    virtual ~JOfficeBase();

    bool isValid() const;

    OfficeType officeType() const;
    QAxObject *application();
    QAxObject *workbooks();
    QAxObject *workbook(int index);
    QAxObject *activeWorkbook();
    QAxObject *sheets();
    QAxObject *sheet(int index);

    QAxObject *prependSheet(const QString &sheetName);
    QAxObject *appendSheet(const QString &sheetName);
    QAxObject *insertSheet(int index, const QString &sheetName);
    bool setSheetCount(int count);
    bool clearSheet();

    bool create();
    bool open(const QString &filePath);
    bool saveAs(const QString &filePath);
    bool close();
    bool show(bool enabled = true);

    static bool generateDocumentFile(QAxBase *axBase, const QString &filePath = QString());

Q_SIGNALS:

public Q_SLOTS:

private:
    Q_DISABLE_COPY(JOfficeBase)
    J_DECLARE_PRIVATE(JOfficeBase)
};

#endif // JOFFICEBASE_H
