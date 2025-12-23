#include "mainwindow.h"

#include <QApplication>

#include <QDomDocument>
#include <QFile>
#include <QDebug>
#include <QRegularExpression>

#include <QProcess>
#include <QDir>

QString TemplatePath = "C:/Users/denwoken/Documents/Qt/DocumentationGenerator/templates/ClassTemplate.drawio";
QString outputPath = "generated/output.drawio";
QString drawIoPath = "C:/Program Files/draw.io/draw.io.exe";

QRegularExpression rxForSingleParameter("%[^%\\[\\]]+%");
QRegularExpression rxForMultipleParameters("%[^%\\[\\]]+\\[[^\\]]*\\]%");
QRegularExpression rxForName("%([a-zA-Z0-9_]+)(\\[.*\\])?%");
QString extractPropertyName(QString prop){
    auto m = rxForName.match(prop);
    if(m.hasMatch()){
        return m.captured(1);
    }
    return "No specified Name?";
}

QDomElement findCellById(
    const QDomNodeList &cells,
    const QString &id)
{
    for (int i = 0; i < cells.count(); ++i) {
        QDomElement el = cells.at(i).toElement();
        if (el.attribute("id") == id)
            return el;
    }
    return QDomElement();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);




    /*
    QFile templateFile(TemplatePath);
    if(!templateFile.open(QFile::ReadOnly)){
        qDebug() << "open(QFile::ReadOnly) error path:" << TemplatePath;
        return 1;
    }


    QDomDocument doc;
    if(!doc.setContent(&templateFile))
    {
        qDebug() << "Failed to parse XML";
        return 1;
    }
    templateFile.close();


    QDomElement root = doc.documentElement();
    QDomNodeList diagrams = root.elementsByTagName("diagram");
    if(diagrams.isEmpty()){
        qDebug() << "Not found XML root <diagram>";
        return 1;
    }

    QDomElement mxGraphModel = diagrams.at(0).firstChildElement("mxGraphModel");
    QDomElement mxRoot = mxGraphModel.firstChildElement("root");


    QMap<QString, QStringList> parameters = {
        { "className" ,{"The Name of the Class"}},
        { "classFields" ,{"class Field 1", "class Field 2"}},
        { "publicMethods" ,{"public Method 1", "public Method 2"}},
        { "privateMethods" ,{"private Method 1", "private Method 2"}}
    };

    QDomNodeList cells = mxRoot.elementsByTagName("mxCell");
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

            if(parameters.contains(propertyName)){
                QStringList list = parameters[propertyName];
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
            if(parameters.contains(propertyName)){
                QStringList list = parameters[propertyName];
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

    QFileInfo fi(outputPath);
    QDir().mkpath(fi.absolutePath());
    QFile outputFile(outputPath);
    if(!outputFile.open(QFile::WriteOnly)){
        qDebug() << "open(QFile::WriteOnly) error path:" << outputPath;
        return 1;
    }
    outputFile.resize(0);

    QTextStream out(&outputFile);
    doc.save(out, 2, QDomNode::EncodingFromDocument);
    outputFile.close();









    QStringList arguments;
    arguments << "--export";
    arguments << "--scale" << "4";
//    arguments << "--disable-gpu";
    arguments << "--no-sandbox";
    //arguments << "--background" << "#ffffff";
    arguments << "--format" << "jpg";
    arguments << "--output" << fi.absoluteFilePath()+".jpg" ;
    arguments << fi.absoluteFilePath();

    qDebug() << arguments.join(" ");

    QProcess process;
    process.start(drawIoPath, arguments);
    process.waitForFinished();

    qDebug() << process.readAllStandardOutput();
    qDebug() << process.readAllStandardError();


*/

    MainWindow w;
    w.show();
    return a.exec();
}
