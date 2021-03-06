#ifndef BACKYARDBRAINS_PLAYER_H
#define BACKYARDBRAINS_PLAYER_H

#include <bass.h>
#include <sigslot.h>
#include <stdint.h>

namespace BackyardBrains {

class Player : public sigslot::has_slots<> {
public:

	Player();
	~Player();

	void start(int sampleRate);

	bool paused() const;
	void setPaused(bool paused);
	int volume() const;
	void setVolume(int volume);
	int sampleRate() const;
	void setSampleRate(int sampleRate);

	void push(void *data, uint32_t size);
	void stop();
    uint64_t pos(uint16_t bytesPerSample, uint16_t numOfChannels);
    void setPos(uint64_t npos, uint16_t bytesPerSample, uint16_t numOfChannels);
    int stateOfBuffer();
private:
	HSTREAM _output;
};

}

#endif
