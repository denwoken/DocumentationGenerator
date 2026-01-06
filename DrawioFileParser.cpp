#include "DrawioFileParser.h"
#include "qregularexpression.h"
#include <QFile>
#include <QDebug>

DrawioFileParser::DrawioFileParser(QObject *parent)
    : QObject{parent}
{}

bool DrawioFileParser::loadDrawioFile(const QString &path)
{
    QFile templateFile(path);
    if(!templateFile.open(QFile::ReadOnly)){
        qDebug() << "open(QFile::ReadOnly) error path:" << path;
        return 1;
    }
    m_documentTemplate.clear();
    if(!m_documentTemplate.setContent(&templateFile))
    {
        qDebug() << "Failed to parse drawio XML";
        return 1;
    }
    templateFile.close();
    m_documentOutput = m_documentTemplate;

    return 0;
}

const QDomDocument* DrawioFileParser::getDrawioDocumentTemplate() const
{
    return &m_documentTemplate;
}
const QDomDocument* DrawioFileParser::getDrawioDocumentOutput() const
{
    return &m_documentOutput;
}

void DrawioFileParser::setPlaceHoldersParameters(const QMap<QString, QStringList> &placeHolders){
    m_placeHolders = placeHolders;
}



QRegularExpression rxForSingleParameter("%[^%\\[\\]]+%");
QRegularExpression rxForMultipleParameters("%[^%\\[\\]]+\\[[^\\]]*\\]%");
QRegularExpression rxForName("%([a-zA-Z0-9_]+)(\\[.*\\])?%");


bool DrawioFileParser::insertPlaceHolders(){
    m_documentOutput = m_documentTemplate;

#define LOCAL_ASSERT(expr, err_message) \
    if(!expr) {                         \
        qCritical() << err_message;     \
        return 1;                       \
    }                                   \


    QDomElement root = m_documentOutput.documentElement();
    LOCAL_ASSERT(!root.isNull(), "documentElement is not exist");

    QDomNodeList diagrams = root.elementsByTagName("diagram");
    LOCAL_ASSERT(!diagrams.isEmpty(),"Not found XML <diagram>")
    LOCAL_ASSERT(diagrams.size(), "<diagrams> contain nothing");

    QDomElement mxGraphModel = diagrams.at(0).firstChildElement("mxGraphModel");
    LOCAL_ASSERT(!diagrams.isEmpty(), "Not found XML <mxGraphModel>");

    QDomElement mxRoot = mxGraphModel.firstChildElement("root");
    LOCAL_ASSERT(!mxRoot.isNull(), "Not found XML <root>");


    QDomNodeList cells = mxRoot.elementsByTagName("mxCell");
    LOCAL_ASSERT(!cells.isEmpty(), "Not found XML <root>");


    int i=0;
    while(i < cells.count())
    {
        QDomElement cell = cells.at(i).toElement();
        QString val = cell.attribute("value");

        QRegularExpressionMatch singleMatch = rxForSingleParameter.match(val);
        QRegularExpressionMatch multipleMatch = rxForMultipleParameters.match(val);

        if(singleMatch.hasMatch()){
            QString propertyName = extractPropertyName(val);
            qDebug() << "Имя плейсхолдера singleMatch:" << propertyName;

            auto iter = m_placeHolders.find(propertyName);
            if(iter != m_placeHolders.end()){
                QStringList list = *iter;
                if(list.size()>0) {
                    int len = singleMatch.capturedLength(0);
                    int start = singleMatch.capturedStart(0);

                    QString newVal = val;
                    newVal.remove(start, len);
                    newVal.insert(start, list[0]);

                    cell.setAttribute("value", newVal);
                }
            }
        }



        if(multipleMatch.hasMatch()){
            QString propertyName = extractPropertyName(val);
            qDebug() << "Имя плейсхолдера multipleMatch:" << propertyName;

            // множественный плейсхолдер: копируем ноду на каждую строку
            auto iter = m_placeHolders.find(propertyName);
            if(iter != m_placeHolders.end()){
                QStringList list = *iter;
                QDomNode parentNode = cell.parentNode();
                QDomNode prevNode = cell;
                QDomElement prevGeom = cell.firstChildElement("mxGeometry");
                int spacing = prevGeom.attribute("height").toDouble();

                int cloneIndex = 0;
                int offset = prevGeom.attribute("y").toDouble();// тк удаляю шаблонный элемент
                for(const QString &item : list){
                    QDomElement newCell = cell.cloneNode().toElement();

                    newCell.setAttribute("id", cell.attribute("id") + "_" + QString::number(cloneIndex++));

                    int len = multipleMatch.capturedLength(0);
                    int start = multipleMatch.capturedStart(0);

                    QString newVal = val;
                    newVal.remove(start, len);
                    newVal.insert(start, item);
                    newCell.setAttribute("value", newVal);

                    // смещаем вниз
                    QDomElement geom = newCell.firstChildElement("mxGeometry");
                    if(!geom.isNull()){
                        //double y = geom.attribute("y").toDouble();
                        geom.setAttribute("y", offset);
                        offset += spacing;
                    }

                    parentNode.insertAfter(newCell, prevNode);
                    prevNode = newCell;

                }

                // удаляем исходную ноду с плейсхолдером
                parentNode.removeChild(cell);

                // подпрвляем остальные элементы
                QDomElement nextNode = prevNode.toElement().nextSiblingElement();
                while(!nextNode.isNull()){
                    QDomElement geom = nextNode.firstChildElement("mxGeometry");
                    if(!geom.isNull()){
                        geom.setAttribute("y", offset);
                        double h = geom.attribute("height").toDouble();
                        offset += h;
                    }
                    nextNode = nextNode.nextSiblingElement();
                }

                int totalAdjustment = spacing * (list.size()-1);

                QDomElement parent = findCellById(prevNode.parentNode().childNodes(),
                                                  prevNode.toElement().attribute("parent"));
                if(!parent.isNull()){
                    QDomElement parentGeom = parent.firstChildElement("mxGeometry");
                    if (!parentGeom.isNull()) {
                        double h = parentGeom.attribute("height").toDouble();
                        parentGeom.setAttribute("height", h + totalAdjustment);
                    }
                }

                i += list.size() -1;
            }
        }
        i++;






    }


#undef LOCAL_ASSERT

    return 0;
}

QString DrawioFileParser::extractPropertyName(QString prop){
    auto m = rxForName.match(prop);
    if(m.hasMatch()){
        return m.captured(1);
    }
    return "No specified Name?";
}

QDomElement DrawioFileParser::findCellById(const QDomNodeList &cells, const QString &id)
{
    for (int i = 0; i < cells.count(); ++i) {
        QDomElement el = cells.at(i).toElement();
        if (el.attribute("id") == id)
            return el;
    }
    return QDomElement();
}
