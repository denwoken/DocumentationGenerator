#ifndef STATUSLAMP_H
#define STATUSLAMP_H

#include "qboxlayout.h"
#include "qlabel.h"
#include <QWidget>

/*
 * QStatusBar это виджет, предоставляемый библиотекой Qt
 *  для создания строки состояния в графическом пользовательском интерфейсе.
 *  Он используется для вывода различной информации о состоянии приложения
 *  или текущих операциях.
 *
 *  В QStatusBar можно добавлять виджеты с помощью методов
 *  addWidget для размещения их в левой части строки состояния и
 *  addPermanentWidget для размещения в правой части строки состояния.
 *
 *  Пример использования QStatusBar и StatusLamp:
 *
    QStatusBar *stbar = new QStatusBar();
    setStatusBar(stbar);
    QWidget * widget = new QWidget(stbar);
    widget->resize(500, height());
    widget->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    QProgressBar *prg_bar = new QProgressBar(widget);
    QPushButton *leftBut  = new QPushButton("Left", widget);
    QPushButton *rightBut = new QPushButton("Right", widget);

    prg_bar->setValue(20);
    prg_bar->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    prg_bar->setTextVisible(false);
    prg_bar->setFixedWidth(100);

    QHBoxLayout *layout   = new QHBoxLayout(widget);
    layout->setMargin(0);
    layout->addWidget(leftBut, 0);
    layout->addWidget(prg_bar, 0);
    layout->addWidget(rightBut, 0);

    stbar->addWidget(widget);// для добавления в левой части строки состояния

    StatusLamp *lamp = new StatusLamp();
    lamp->setText("lamp");
    lamp->setLampState(LampState::LampStateOn);// == lamp->turnOn();
    lamp->setLampRadius(4);

    connect(leftBut, &QPushButton::clicked,
            lamp, &StatusLamp::toggle);
    connect(rightBut, &QPushButton::pressed,
            lamp, &StatusLamp::disable);
    connect(rightBut, &QPushButton::released,
            lamp, &StatusLamp::turnOn);

    stbar->addPermanentWidget(lamp, 0); // для добавления в правой части строки состояния
 */

/*!
 * \brief  LampState Перечисление описывает все возожные состояния лампы
 */
enum class LampState {
    LampStateOff,
    LampStateOn,
    LampStateMedium,
    LampStateDisable,

    LampStateMax = LampStateDisable,
};

class StatusLamp : public QWidget
{
    Q_OBJECT
public:
    /*!
     * \brief StatusLamp Инициализирует лампу со значениями поумолчанию:
     *     color off <- QColor(250,50,50);
     *     color on <- QColor(50,250,50);
     *     color medium <- QColor(250,250,50);
     *     color disable <- QColor(50,50,50);
     *     color border <- QColor(150,150,150);
     *
     *     lamp size <- QSize(20, 20);
     *     lamp radius <- 4;
     *     lamp state <- LampStateOff;
     *     lamp text <- ""
     */

    explicit StatusLamp(QWidget *parent=nullptr);

    /*!
     * \brief setLampSize Задает размер лампы в пикселях
     */
    void setLampSize(const QSize size);
    /*!
     * \brief setLampRadius задает радиус скругления лампы в пикселях
     */
    void setLampRadius(int radius);
    /*!
     * \brief setText изменяет текст перед лампой
     */
    void setText(const QString &str);
    /*!
     * \brief setLampToolTip изменяет всплывающую подсказку при наведении
     */
    void setLampToolTip(const QString &str);

    /*!
     * \brief setColorState Выставляет цвет лампы согласно ее состоянию
     */
    void setColorState(const QColor &color, const LampState state);
    /*!
     * \brief setColorBorder Задает цвет границы лампы
     */
    void setColorBorder(const QColor &color);
    /*!
     * \brief setColorOn Задает цвет лампы в состоянии LampStateOn
     */
    void setColorOn(const QColor &color);
    /*!
     * \brief setColorOff Задает цвет лампы в состоянии LampStateOff
     */
    void setColorOff(const QColor &color);
    /*!
     * \brief setColorMedium Задает цвет лампы в состоянии LampStateMedium
     */
    void setColorMedium(const QColor &color);
    /*!
     * \brief setColorDisable Задает цвет лампы в состоянии LampStateDisable
     */
    void setColorDisable(const QColor &color);

    /*!
     * \brief setLampState устанавливает состояние лампы согласно аргументу LampState state
     */
    void setLampState(LampState state);


    /*!
     * \brief setLampStateOn изменяет состояние лампы на LampStateOn
     */
    void setLampStateOn();
    /*!
     * \brief setLampStateOff изменяет состояние лампы на LampStateOff
     */
    void setLampStateOff();
    /*!
     * \brief setLampStateMedium изменяет состояние лампы на LampStateMedium
     */
    void setLampStateMedium();
    /*!
     * \brief setLampStateDisable изменяет состояние лампы на LampStateDisable
     */
    void setLampStateDisable();

    /*!
     * \brief disable изменяет состояние лампы на противоположное
     * LampStateOff -> LampStateOn
     * LampStateOn -> LampStateOff
     */
    void toggle();
    /*!
     * \brief getLampState возвращает текущее состояние лампы
     */
    LampState getLampState() const;

signals:
    /*!
     * \brief sigTurnedOn сигнал генерируется только при смене состояния лампы на LampStateOn
     */
    void sigTurnedOn();
    /*!
     * \brief sigTurnedOff сигнал генерируется только при смене состояния лампы на LampStateOff
     */
    void sigTurnedOff();
    /*!
     * \brief sigMediumActivated сигнал генерируется только при смене состояния лампы на LampStateMedium
     */
    void sigMediumActivated();
    /*!
     * \brief sigDisabled сигнал генерируется только при смене состояния лампы на LampStateDisable
     */
    void sigDisabled();
    /*!
     * \brief sigStateChanged сигнал генерируется при смене состояния лампы
     * \param state
     */
    void sigStateChanged(LampState state);

private:
    /*!
     * \brief updateStyleSheet Обновляет StyleSheet лампы
     *  применяет цвета, заданные размеры и т.д.
     */
    void updateStyleSheet();

    QHBoxLayout *m_hlayout ;
    QLabel *m_textLabel;
    QLabel *m_indicatorLabel;

    LampState m_lampState;
    int m_lampRadius;
    QSize m_lampSize;

    struct {
        QColor Off;
        QColor On;
        QColor Medium;
        QColor Disable;

        QColor Border;
    }m_colors;

};

#endif // STATUSLAMP_H
