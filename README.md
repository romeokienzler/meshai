# meshai

In disasters, communication infrastructure is the first to fail, although it is a crucial requirement for effective organization of help and resource allocation.

We build a robust system for effective and rapid deployment of wireless mesh communication for disaster relief. In communication breakdown scenarios our system can be deployed within hours.
 
We have tested various mesh technologies and finally decided to use a Sub-GHZ mesh communication protocol based on the LoRa standard – providing cheap and low-power requiring mesh nodes to build a resilient mesh-network for robust telemetry and messaging without relying on any existing communication infrastructure. 

We’ve defined different types of solar powered nodes which can be deployed in-flight using aviation systems (airplanes, drones or helicopters) on parachutes.

C-Nodes enable Wifi access points and Bluetooth connections to interact with smartphones. We run a webserver on the nodes, so no apps need to be installed.

I-Nodes provide satellite (Iridium) uplinks to the mesh.

T-Nodes can measure the environment using attached sensors.

Due to the very low power requirement, we don’t rely on solar power only, since a substantial amount of energy can be provided by the batteries.

 In order to efficiently distribute the nodes, the drop rate (and therefor the effective spatial node distribution) is a crucial factor for successful establishment of communication.


LoRa range is subject to a huge variation based on the environment (building, ground topology). We propose a DeepLearning based spatial range prediction model based on Google Maps tiles mapped to range experiments from The Things Network (ttnmapper.org). This model predicts achievable ranges for an arbitrary point of any unseen tile of the world map. We’ve build the model using the Keras/TensorFlow framework using IBM Watson Studio and successfully deployed it on IBM Watson Machine Learning. 

The backbone of this network is powered by the IBM Watson IoT Platform. 

A successful establishment of such a mesh communication infrastructure enables a broad set of use cases related to disaster relief. 

