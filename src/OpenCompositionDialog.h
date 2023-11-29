#ifndef OPENCOMPOSITIONDIALOG_H
#define OPENCOMPOSITIONDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QComboBox>
#include <QStringList>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QWidget>
#include <QFileDialog>
#include <QMessageBox>

class OpenCompositionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OpenCompositionDialog(QWidget *parent);
    ~OpenCompositionDialog();

    class CompositionOpenMode
    {
    public:
        QString comboBoxText;
        QString labelText[2];
        QString fileDialogCaption[2];
        QString fileDialogFilter[2];
        bool needsRow1;
        bool isEnabled;

        CompositionOpenMode(QString comboBoxText,
                            QString labelText1, QString labelText2,
                            QString fileDialogCaption1, QString fileDialogCaption2,
                            QString fileDialogFilter1, QString fileDialogFilter2,
                            bool needsRow1,
                            bool isEnabled = true):
            comboBoxText(comboBoxText),
            labelText{labelText1, labelText2},
            fileDialogCaption{fileDialogCaption1, fileDialogCaption2},
            fileDialogFilter{fileDialogFilter1, fileDialogFilter2},
            needsRow1(needsRow1),
            isEnabled(isEnabled)
        {}

        bool operator==(const CompositionOpenMode& m)
        {
            return this->comboBoxText == m.comboBoxText;
        }

        static QStringList getTexts(const QList<CompositionOpenMode>& openModes)
        {
            QStringList list;
            list.reserve(openModes.size());

            for (const CompositionOpenMode& m : openModes)
            {
                list.append(m.comboBoxText);
            }

            return list;
        }
    };

    /**
     * @brief Enum definition for the composition open mode. MUST be kept in sync with the openModes QList<CompositionOpenMode>!
     */
    enum class CompositionOpenModeResult
    {
        None = -1,
        AUDIO_ONLY = 0,
        AUDIO_LIGHT_DATA = 1,
        AUDIO_AUDACITY_LABELS = 2
    };
    /**
     * @brief Holds the composition mode after the user confirmed this selection.
     */
    CompositionOpenModeResult openModeResult = CompositionOpenModeResult::None;
    /**
     * @brief Holds the result of the row 0
     */
    QString row0Result = "";
    /**
     * @brief Holds the result of the row 1
     */
    QString row1Result = "";

public slots:
    /**
     * @brief Will be called by the void QDialogButtonBox::accepted() signal.
     */
    void buttonBox_onAccepted();
    /**
     * @brief Will be called by the void QDialogButtonBox::rejected() signal.
     */
    void buttonBox_onRejected();

    /**
     * @brief Will be called by the void QComboBox::currentIndexChanged(int index) signal.
     * @param index
     */
    void comboBox_openMode_onCurrentIndexChanged(int index);

    /**
     * @brief Will be called by the void QPushButton::clicked(bool checked) signal of the row0FileButton.
     * @param checked If the action is checked.
     */
    void browseRow0FileButton_onClicked(bool checked);
    /**
     * @brief Will be called by the void QPushButton::clicked(bool checked) signal of the row1FileButton.
     * @param checked If the action is checked.
     */
    void browseRow1FileButton_onClicked(bool checked);

private:
    const QList<CompositionOpenMode> openModes = {
        CompositionOpenMode("Audio file (ogg)", "Audio file:", QString(), "Select Audio (ogg)", "", "*.ogg", "", false, false),
        CompositionOpenMode("Audio file (ogg) + Light data file (glypha)", "Audio file:", "Light data file:", "Select Audio (ogg)", "Select Light data (glypha)", "*.ogg", "*.glypha", true, true),
        CompositionOpenMode("Audio file (ogg) + Audacity Labels file (txt)", "Audio file:", "Audacity Labels file:", "Select Audio (ogg)", "Select Audacity Labels (txt)", "*.ogg", "*.txt", true, false)
    };

    QDialogButtonBox *buttonBox;
    QComboBox *comboBox_openMode;
    QWidget* row0Container[3];
    QWidget* row1Container[3];


    /**
     * @brief Enable/Disable an item in the QComboBox. When an item is disabled it is not selectable any more.
     * @param comboBox Which QComboBox.
     * @param index Which item.
     * @param enabled true: Enable the item; false: Disable the item
     */
    void setComboBoxItemEnabled(const QComboBox* comboBox, int index, bool enabled);

    /**
     * @brief Enable/Disable the whole audioFileRow in the QFormLayout
     * @param enabled true: Enable all the items; false: Disable all the items
     */
    void setRow0Enabled(bool enabled);
    /**
     * @brief Enable/Disable the whole lightDataFileRow in the QFormLayout
     * @param enabled true: Enable all the items; false: Disable all the items
     */
    void setRow1Enabled(bool enabled);
};

#endif // OPENCOMPOSITIONDIALOG_H
