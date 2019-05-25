![muse eeg artwork](https://raw.githubusercontent.com/laurames/signal/master/3.JPEG)

# Signal: Muse EEG headband interactive artwork

This is an interactive artwork using the [Muse2014 headband](https://choosemuse.com/muse/). The artwork is built on top of [MuseIO](http://developer.choosemuse.com/tools/available-data) to access data and [Muse command line tools](http://developer.choosemuse.com/tools/museplayer/command-line-options) to send data via OSC to [OpenFrameWorks](https://openframeworks.cc).

The exhibition website [Ristiaallokko/Crosswaves](https://risti.aallokko.art/)
The facebook page [Ristiaallokko/Crosswaves](https://www.facebook.com/events/2364148737164552/)

![artwork](https://raw.githubusercontent.com/laurames/signal/master/2.JPG)

## Artwork description

A landscape of people's thoughts.

Signal is a collaborative artwork which builds a landscape from the EEG collected from individuals’ current mental state. The thoughts are collected while the person is viewing the artwork.

### Prerequisites

This project was created using the tools for Mac. You need to have an original muse headset (version from 2014).

### Running

With MuseIO installed from [tools](http://developer.choosemuse.com/tools) run in terminal with the correct name of the device that is something similar to "Muse-1111":

```
muse-io --device Muse-**** --osc osc.udp://localhost:5000

```

Open the project in OpenFrameWorks and run.

## Built With

* [OpenFrameWorks](https://openframeworks.cc)
* [MuseIO](https://maven.apache.org/) - A driver to connect to Muse 2014 over Bluetooth and then stream Muse data over OSC
* [Muse command line tools](http://developer.choosemuse.com/tools/museplayer/command-line-options) - Command Line Options

## Acknowledgments

* Hat tip to anyone whose code was used
