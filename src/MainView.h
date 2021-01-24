#ifndef BACKYARDBRAINS_MAINVIEW_H
#define BACKYARDBRAINS_MAINVIEW_H

#include "widgets/Widget.h"
#include <ctime>
#include "engine/ArduinoSerial.h"
#include "widgets/BoxLayout.h"
#include "ControlServer.h"

namespace BackyardBrains {

class RecordingManager;
class AnalysisManager;
class FileRecorder;
class AudioView;
class RecordingBar;
class AnalysisView;
class FFTView;
class ControlServer;

namespace Widgets {
	class PushButton;
	class ScrollBar;
}

class MainView : public Widgets::Widget {
public:
	MainView(RecordingManager &mngr, AnalysisManager &anaman, FileRecorder &fileRec, Widget *parent = NULL);

    struct SerialPortIndicator
    {
        ArduinoSerial::SerialPort serialPort;
        bool connected;
        Widgets::PushButton * button;
        SerialPortIndicator()
        {
            button = NULL;
            connected = false;
        }
    };

	~MainView();

	void recordPressed();
	void startRecord();
	void startRecord(std::string record_path);
	void stopRecord();
	void addMarker(std::string marker);
	void shutdown();

private:
	RecordingManager &_manager;
	AnalysisManager &_anaman;
	FileRecorder &_fileRec;

    std::list<SerialPortIndicator> shieldButtons;


	Widgets::PushButton *_pauseButton;
	Widgets::PushButton *_ekgButton;
	Widgets::PushButton *_configButton;
	Widgets::PushButton *_fileButton;
	Widgets::PushButton *_forwardButton;
	Widgets::PushButton *_recordButton;
	Widgets::PushButton *_fftButton;
	Widgets::PushButton *_analysisButton;
	Widgets::PushButton *_muscleHIDButton;
    Widgets::PushButton *_neuronHIDButton;
	Widgets::PushButton *threshButton;

    Widgets::BoxLayout * shieldsButtonBoxLayout;
   // Widgets::PushButton *_plantSSButton;
   // Widgets::PushButton *_muscleSSButton;
   // Widgets::PushButton *_heartSSButton;

    void plantPressed(Widgets::MouseEvent *mouseEv, Widgets::PushButton* buttonInst);
    void musclePressed(Widgets::MouseEvent *mouseEv, Widgets::PushButton* buttonInst);
    void heartPressed(Widgets::MouseEvent *mouseEv, Widgets::PushButton* buttonInst);
    void neuronPressed(Widgets::MouseEvent *mouseEv, Widgets::PushButton* buttonInst);
    Widgets::PushButton *_alphaFeedbackButton;
    Widgets::PushButton * _backwardButton;
    void alphaFeedbackPressed();
    bool alphaFeedbackAcive;

	Widgets::ScrollBar *_seekBar;
	Widgets::Widget *_threshavgGroup;
	AudioView *_audioView;
	RecordingBar *_recBar;
	AnalysisView *_anaView;
	FFTView *_fftView;

	ControlServer * _server;

	// Whether to override the default getRecordingPath()
	bool _recording_path_override;
	std::string _recording_path;

	void keyPressEvent(Widgets::KeyboardEvent *e);

	/* slots: */
	void pausePressed();
	void backwardPressed();
	void forwardPressed();

	void ekgPressed();

	void threshPressed();
	void filePressed();
	void configPressed();
	void analysisPressed();

    void muscleHIDPressed();
    void neuronHIDPressed();

	void fftPressed();

    void paintEvent();
    clock_t timerUSB;
    int timerForAlphaWaveMessages = 0;
    void triggerEvent();

    void drawTimeLabelsForFile();

    void connectToFirstShieldOfType(ArduinoSerial::SerialDevice deviceType);
    void connectToShieldForButton(Widgets::PushButton* buttonInst);

	void advance();

};

}
#endif
