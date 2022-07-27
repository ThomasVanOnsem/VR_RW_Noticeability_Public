# Latency Code

The latency experiments contained 2 elements:
* The custom SDK running on the HMD. More specifically, the file
 [VrCubeWorld_Framework.cpp](Custom_SDK/VrSamples/VrCubeWorld_Framework/Src/VrCubeWorld_Framework.cpp)
was used to run the experiment. Downloading and installing the SDK was 
done using the info provided by Oculus at https://developer.oculus.com/downloads/package/oculus-mobile-sdk/

* The UDP receiver running on a laptop, ready to receive the packets from
the custom SDK on the HMD. This is a C++ program that listens for incoming
packets and displays them.