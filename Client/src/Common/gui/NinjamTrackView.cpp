#include "NinjamTrackView.h"
#include "BaseTrackView.h"
#include "PeakMeter.h"
#include <QLineEdit>
#include <QLabel>
#include <QDebug>
#include <QGroupBox>
#include <QPushButton>
#include <QGridLayout>
#include <QSlider>
#include <QStyle>
#include "MainController.h"
#include "Utils.h"

// +++++++++++++++++++++++++
NinjamTrackView::NinjamTrackView(Controller::MainController *mainController, long trackID) :
    BaseTrackView(mainController, trackID),
    orientation(Qt::Vertical)
{
    channelNameLabel = new MarqueeLabel();
    channelNameLabel->setObjectName("channelName");
    channelNameLabel->setText("");
    channelNameLabel->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));

    chunksDisplay = new IntervalChunksDisplay(this);
    chunksDisplay->setObjectName("chunksDisplay");
    chunksDisplay->setVisible(false);

    setupVerticalLayout();

    setUnlightStatus(true); // disabled/grayed until receive the first bytes.
}

void NinjamTrackView::refreshStyleSheet()
{
    style()->unpolish(channelNameLabel);
    style()->polish(channelNameLabel);
    BaseTrackView::refreshStyleSheet();
}

void NinjamTrackView::setInitialValues(const Persistence::CacheEntry &initialValues)
{
    cacheEntry = initialValues;

    // remember last track values
    levelSlider->setValue(initialValues.getGain() * 100);
    panSlider->setValue(initialValues.getPan() * panSlider->maximum());
    if (initialValues.isMuted())
        muteButton->click();
    if (initialValues.getBoost() < 1) {
        buttonBoostMinus12->click();
    } else {
        if (initialValues.getBoost() > 1)
            buttonBoostPlus12->click();
        else
            buttonBoostZero->click();
    }
}

// +++++++++++++++
void NinjamTrackView::updateGuiElements()
{
    BaseTrackView::updateGuiElements();
    channelNameLabel->updateMarquee();
}

void NinjamTrackView::setUnlightStatus(bool status)
{
    BaseTrackView::setUnlightStatus(status);
    chunksDisplay->setVisible(!status && chunksDisplay->isVisible());

    if (status) {// remote user stop xmiting and the track is greyed/unlighted?
        Audio::AudioNode *trackNode = mainController->getTrackNode(getTrackID());
        if (trackNode)
            trackNode->resetLastPeak(); // reset the internal node last peak to avoid getting the last peak calculated when the remote user was transmiting.
    }
}

QSize NinjamTrackView::sizeHint() const
{
    if (orientation == Qt::Horizontal)
        return QWidget::sizeHint();
    return BaseTrackView::sizeHint();
}

void NinjamTrackView::setOrientation(Qt::Orientation newOrientation)
{
    if (this->orientation == newOrientation)
        return;

    this->orientation = newOrientation;
    if (newOrientation == Qt::Horizontal)
        setupHorizontalLayout();
    else
        setupVerticalLayout();

    setProperty("horizontal", newOrientation == Qt::Horizontal ? true : false);
    refreshStyleSheet();
}

void NinjamTrackView::setupVerticalLayout()
{
    BaseTrackView::setupVerticalLayout();

    mainLayout->removeWidget(channelNameLabel);
    mainLayout->removeItem(primaryChildsLayout);
    mainLayout->removeItem(secondaryChildsLayout);
    mainLayout->removeWidget(chunksDisplay);

    mainLayout->addWidget(channelNameLabel, 0, 0, 1, 2);// insert channel name label in top
    mainLayout->addLayout(primaryChildsLayout, 1, 0);
    mainLayout->addLayout(secondaryChildsLayout,1, 1);
    mainLayout->addWidget(chunksDisplay, 2, 0, 1, 2); // append chunks display in bottom

    primaryChildsLayout->setDirection(QBoxLayout::TopToBottom);
    secondaryChildsLayout->setDirection(QBoxLayout::TopToBottom);

}

void NinjamTrackView::setupHorizontalLayout()
{
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    mainLayout->removeWidget(channelNameLabel);
    mainLayout->removeItem(primaryChildsLayout);
    mainLayout->removeItem(secondaryChildsLayout);
    mainLayout->removeWidget(chunksDisplay);

    mainLayout->addWidget(channelNameLabel, 0, 0);
    mainLayout->addLayout(primaryChildsLayout, 0, 1);
    mainLayout->addLayout(secondaryChildsLayout, 1, 0, 1, 2);
    mainLayout->addWidget(chunksDisplay, 1, 0, 1, 2); // append chunks display in bottom overlapping the other widgets

    mainLayout->setContentsMargins(3, 3, 3, 3);
    mainLayout->setVerticalSpacing(6);

    primaryChildsLayout->setDirection(QBoxLayout::RightToLeft);
    secondaryChildsLayout->setDirection(QBoxLayout::LeftToRight);

    levelSlider->setOrientation(Qt::Horizontal);
    levelSliderLayout->setDirection(QBoxLayout::RightToLeft);

    boostWidgetsLayout->setDirection(QHBoxLayout::LeftToRight);

    peakMeterLeft->setOrientation(Qt::Horizontal);
    peakMeterLeft->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
    peakMeterRight->setOrientation(Qt::Horizontal);
    peakMeterRight->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum));
    metersLayout->setDirection(QBoxLayout::TopToBottom);
    meterWidgetsLayout->setDirection(QBoxLayout::LeftToRight);

    muteSoloLayout->setDirection(QHBoxLayout::LeftToRight);
}


QPoint NinjamTrackView::getDbValuePosition(const QString &dbValueText,
                                           const QFontMetrics &metrics) const
{
    if (orientation == Qt::Vertical)
        return BaseTrackView::getDbValuePosition(dbValueText, metrics);

    float sliderPosition = (double)levelSlider->value()/levelSlider->maximum();
    int textX = sliderPosition * levelSlider->width() + levelSlider->x()
                - metrics.width(dbValueText) - FADER_HEIGHT;
    int textY = levelSlider->y() + levelSlider->height() + 3;
    return QPoint(textX, textY);
}

void NinjamTrackView::finishCurrentDownload()
{
    if (chunksDisplay->isVisible())
        chunksDisplay->startNewInterval();
}

void NinjamTrackView::incrementDownloadedChunks()
{
    chunksDisplay->incrementDownloadedChunks();
}

void NinjamTrackView::setDownloadedChunksDisplayVisibility(bool visible)
{
    if (chunksDisplay->isVisible() != visible) {
        chunksDisplay->reset();
        chunksDisplay->setVisible(visible);
        if(orientation == Qt::Horizontal)
            BaseTrackView::setLayoutWidgetsVisibility(secondaryChildsLayout, !visible);
        else
            BaseTrackView::setLayoutWidgetsVisibility(secondaryChildsLayout, true);//secondary widgets are always visible in vertical layout
    }
}

void NinjamTrackView::setChannelName(const QString &name)
{
    this->channelNameLabel->setText(name);
    int nameWidth = this->channelNameLabel->fontMetrics().width(name);
    if (nameWidth <= this->channelNameLabel->contentsRect().width())
        this->channelNameLabel->setAlignment(Qt::AlignCenter);
    else
        this->channelNameLabel->setAlignment(Qt::AlignLeft);
    this->channelNameLabel->setToolTip(name);
}

void NinjamTrackView::setPan(int value)
{
    BaseTrackView::setPan(value);
    cacheEntry.setPan(mainController->getTrackNode(getTrackID())->getPan());
    mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
}

void NinjamTrackView::setGain(int value)
{
    BaseTrackView::setGain(value);
    cacheEntry.setGain(value/100.0);
    mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
}

void NinjamTrackView::toggleMuteStatus()
{
    BaseTrackView::toggleMuteStatus();
    cacheEntry.setMuted(mainController->getTrackNode(getTrackID())->isMuted());
    mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
}

void NinjamTrackView::updateBoostValue()
{
    BaseTrackView::updateBoostValue();
    Audio::AudioNode *trackNode = mainController->getTrackNode(getTrackID());
    if (trackNode) {
        cacheEntry.setBoost(trackNode->getBoost());
        mainController->getUsersDataCache()->updateUserCacheEntry(cacheEntry);
    }
}
