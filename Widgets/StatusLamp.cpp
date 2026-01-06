#include "StatusLamp.h"
#include "qdebug.h"

StatusLamp::StatusLamp(QWidget *parent): QWidget(parent){

    m_colors.Off = QColor(250,90,90);
    m_colors.On = QColor(0,150,80);
    m_colors.Medium = QColor(220,220,0);
    m_colors.Disable = QColor(150,150,150);
    m_colors.Border = QColor(100,100,100);

    m_lampSize = QSize(20, 20);
    m_lampRadius = 4;
    m_lampState = LampState::LampStateOff;

    m_indicatorLabel = new QLabel("", this);
    m_indicatorLabel->setFixedSize(m_lampSize);
    m_indicatorLabel->setSizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);

    m_textLabel = new QLabel("", this);
    m_textLabel->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Ignored);
    m_textLabel->hide();

    m_hlayout = new QHBoxLayout(this);
    setLayout(m_hlayout);
    m_hlayout->setMargin(0);
    m_hlayout->addWidget(m_indicatorLabel, 0);
    m_hlayout->addWidget(m_textLabel, 0);

    updateStyleSheet();

}

void StatusLamp::setLampSize(const QSize size){
    if(size.height()<0 || size.width()<0) {
        qWarning("height and width must be >= 0");
        return;
    }

    if(m_lampSize == size ) return;
    m_lampSize = size;
    m_indicatorLabel->setFixedSize(size);
    updateStyleSheet();
}

void StatusLamp::setLampRadius(int radius){
    if(radius<0) {
        qWarning("border radius must be >= 0");
        return;
    }

    if(m_lampRadius == radius) return;
    m_lampRadius = radius;
    updateStyleSheet();
}

void StatusLamp::setColorBorder(const QColor &color)
{
    if(m_colors.Border == color)return;
    m_colors.Border = color;
    updateStyleSheet();
}

void StatusLamp::setText(const QString &str)
{
    if(str == "")
    {
        m_textLabel->hide();
    }
    else
    {
        m_textLabel->show();
        m_textLabel->setText(str);
    }
}

void StatusLamp::setLampToolTip(const QString &str)
{
    // m_indicatorLabel->setToolTip(str);
    // m_textLabel->setToolTip(str);
    QWidget::setToolTip(str);
}

void StatusLamp::setColorState(const QColor &color, const LampState state)
{
    if(state > LampState::LampStateMax) {
        qWarning("the value is outside the range of the 'LampState' enum");
        return;
    }

    QColor* color_ptr = reinterpret_cast<QColor*>(&m_colors);
    int ind = static_cast<int>(state);
    color_ptr[ind] = color;
    updateStyleSheet();

}

void StatusLamp::setColorOff(const QColor &color) {
    setColorState(color, LampState::LampStateOff);
}

void StatusLamp::setColorOn(const QColor &color) {
    setColorState(color, LampState::LampStateOn);
}

void StatusLamp::setColorMedium(const QColor &color) {
    setColorState(color, LampState::LampStateMedium);
}

void StatusLamp::setColorDisable(const QColor &color) {
    setColorState(color, LampState::LampStateDisable);
}


void StatusLamp::setLampState(LampState state){
    if(state > LampState::LampStateMax) {
        qWarning("the value is outside the range of the 'LampState' enum");
        return;
    }

    if(m_lampState == state) return;
    m_lampState = state;
    updateStyleSheet();

    // выдает сигнал только если значение лампы изменится
    switch(state){
    case LampState::LampStateOn:
        emit sigTurnedOn();
        break;
    case LampState::LampStateOff:
        emit sigTurnedOff();
        break;
    case LampState::LampStateDisable:
        emit sigDisabled();
        break;
    case LampState::LampStateMedium:
        emit sigMediumActivated();
        break;
    }
    emit sigStateChanged(state);
}


void StatusLamp::setLampStateOn() {
    setLampState(LampState::LampStateOn);
}

void StatusLamp::setLampStateOff() {
    setLampState(LampState::LampStateOff);
}

void StatusLamp::setLampStateMedium() {
    setLampState(LampState::LampStateMedium);
}

void StatusLamp::setLampStateDisable() {
    setLampState(LampState::LampStateDisable);
}

void StatusLamp::toggle(){
    if(m_lampState==LampState::LampStateOff) setLampStateOn();
    else if(m_lampState==LampState::LampStateOn) setLampStateOff();
}

LampState StatusLamp::getLampState() const
{
    return m_lampState;
}

void StatusLamp::updateStyleSheet()
{
    QColor* color_ptr = reinterpret_cast<QColor*>(&m_colors);
    int ind = static_cast<int>(m_lampState);
    QColor color =  color_ptr[ind];

    QColor& b_color = m_colors.Border;

    QString size = QString("width : %1px; height %2px;").arg(m_lampSize.width()).arg(m_lampSize.height());
    QString border = QString("border: 1px solid rgb(%1,%2,%3);").arg(b_color.red()).arg(b_color.green()).arg(b_color.blue());
    QString border_r = QString("border-radius: %1px;").arg(m_lampRadius);
    QString background = QString("background : rgb(%1,%2,%3);").arg(color.red()).arg(color.green()).arg(color.blue());

    if(m_indicatorLabel == nullptr){
        qWarning() << "Еhe m_indicatorLabel object wasn't created !";
        return;
    }
    m_indicatorLabel->setStyleSheet(QString("QLabel { %1 }").arg(size+border+border_r+background));
}

