#pragma once

#include <QObject>
#include <QDomDocument>
#include <QMap>

class DrawioFileParser : public QObject
{
    Q_OBJECT
public:
    explicit DrawioFileParser(QObject *parent = nullptr);
    bool loadDrawioFile(const QString &path);

    const QDomDocument* getDrawioDocumentTemplate() const;
    const QDomDocument* getDrawioDocumentOutput() const;


    void setPlaceHoldersParameters(const QMap<QString, QStringList>& placeHolders);

    bool insertPlaceHolders();


protected:

    static QString extractPropertyName(QString prop);
    static QDomElement findCellById(const QDomNodeList &cells, const QString &id);

private:
    QDomDocument m_documentTemplate;
    QDomDocument m_documentOutput;

    QMap<QString, QStringList> m_placeHolders;


signals:



};
