#pragma once

#include "Config.h"

#include "ListModel.h"

#include "model/Project.h"

#include <array>

class TrackModeListModel : public ListModel {
public:
    TrackModeListModel(Project &project) {
        fromProject(project);
    }

    bool sameAsProject(Project &project) {
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            if (_trackModes[i] != project.track(i).trackMode()) {
                return false;
            }
        }
        return true;
    }

    void fromProject(Project &project) {
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            _trackModes[i] = project.track(i).trackMode();
        }
    }

    void toProject(Project &project) {
        // Cache the new track modes because calling setTrackMode will actually
        // trigger a reload of the track setup page and reinitialize the
        // TrackModeListModel leading to only the first track with a new track
        // mode to be updated.
        Track::TrackMode newTrackModes[CONFIG_TRACK_COUNT];
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            newTrackModes[i] = _trackModes[i];
        }
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            int logicTrack = -1;
            if (newTrackModes[i] != project.track(i).trackMode()) {
                if (project.track(i).trackMode() == Track::TrackMode::Logic) {
                    // reset related logic track inputs
                    logicTrack = i;
                    auto in1 = project.track(i).logicTrack().inputTrack1();
                    if (in1 != -1) {
                        project.track(in1).noteTrack().setLogicTrack(-1);
                        project.track(in1).noteTrack().setLogicTrackInput(-1);
                    }
                    auto in2 = project.track(i).logicTrack().inputTrack2();
                    if (in2 != -1 ) {
                        project.track(in2).noteTrack().setLogicTrack(-1);
                        project.track(in2).noteTrack().setLogicTrackInput(-1);
                    }
                    for (int j = 0; j < CONFIG_TRACK_COUNT; ++j) {
                        if (project.track(j).trackMode() == Track::TrackMode::Note && logicTrack != -1) {
                            if (project.track(j).noteTrack().logicTrack() == logicTrack) {
                                project.track(j).noteTrack().setLogicTrack(-1);
                            }
                        }
                    }
                    logicTrack = -1;
                }
                project.setTrackMode(i, newTrackModes[i]);
            }
        }
    }

    virtual int rows() const override {
        return CONFIG_TRACK_COUNT;
    }

    virtual int columns() const override {
        return 2;
    }

    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            str("Track%d", row + 1);
        } else if (column == 1) {
            str(Track::trackModeName(_trackModes[row]));
        }
    }

    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            _trackModes[row] = ModelUtils::adjustedEnum(_trackModes[row], value);
            if (row < 2 && _trackModes[row] == Track::TrackMode::Logic) {
                _trackModes[row] = Track::TrackMode::Arp;
            }
        }
    }

private:
    std::array<Track::TrackMode, CONFIG_TRACK_COUNT> _trackModes;
};
