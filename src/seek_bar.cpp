#include "seek_bar.h"

SeekBar::SeekBar(QWidget *parent)
    : QSlider{Qt::Orientation::Horizontal, parent}
{
    // Set the style so the seek bar moves directly to the cursor
    this->setStyle(new SeekBarStyle(this->style()));

    // Set other settings
    this->setEnabled(false);
    this->setMinimum(0);
    this->setSingleStep(1000);
    this->setPageStep(5000);
}

/*
 * ==================================
 *             Functions
 * ==================================
 */

void SeekBar::resizeEvent(QResizeEvent *event)
{
    qDebug().nospace() << this->width() << "/(" << this->maximum() << "-" << this->minValue << "+1)*" << this->value() << "="
                       << this->width() / (qreal)(this->maximum() - this->minValue + 1) * this->value();
//    // Calculate the minValue
//    uint minValueOld = this->minValue;
//    this->minValue = (uint)(11.5 * (this->maximum() - this->minValue + 1) / (qreal)this->width());
//    qDebug() << this->minValue;

//    if (this->minValue == minValueOld)
//        return;

//    // Update values
//    this->setActualMaximum(this->maximum() - minValueOld);
//    int actualValue = this->value() - minValueOld;
//    this->setActualValue(actualValue < this->minimum() ? 0 : actualValue);

//    // Emit signal
//    emit this->minValueChanged(this->minValue, minValueOld);
}
