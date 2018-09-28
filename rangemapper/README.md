# Rangemapper

This component takes care of predicting ideal node placements basend on a deeplearning model trained on google maps elevation profile and flat map images targeting against The Things Network (TNN) range mapper data.

The underlying machine learning problem is predicting a 2D regression (range heat map) based on specific and latent features from the google maps images

## Step 1 - Capture TNN range mapper data
Furtuntately, TNN allows for downloading raw experiment (war driving) logs from individual users allowing us for creating range heat maps for individual TNN gateway locations. 

Here is an example of the schema of such experiment data for single user

id
time
nodeaddr
appeui
gwaddr
modulation
datarate
snr
rssi
freq
lat
lon
alt
accuracy
hdop
sats
provider
name
mqtt_topic
user_agent
user_id

And some sample data...

id,time,nodeaddr,appeui,gwaddr,modulation,datarate,snr,rssi,freq,lat,lon,alt,accuracy,hdop,sats,provider,name,mqtt_topic,user_agent,user_id
3057917,2018-08-09 08:00:55,60c5a8ffff71d224,xuyutao,60C5A8FFFE71D210,LORA,SF7BW125,9.00,-34.00,904.900,59.367100,17.073000,0.0,null,null,null,Cayenne LPP,xuyutao,null,ttn_http_integration_v2,yutao.xu@rakconnected.com
3057929,2018-08-09 08:03:51,60c5a8ffff71d225,xuyutao,60C5A8FFFE71D210,LORA,SF8BW125,12.00,-33.00,904.100,59.367100,17.072700,0.0,null,null,null,Cayenne 

## Step 2 - Capture associated map

Having range information of a subset of map tiles it is possible to learn range information of map tiles where range information is present. 

The following map tile illustrates an arbitrary google maps tile

![alt text](https://github.com/romeokienzler/meshai/raw/master/rangemapper/img/gm.png "Logo Title Text 1")

## Step 4 - Add 2D target regression data to the training image

Based on range information taken from TTN mapper we can draw a range map

![alt text](https://github.com/romeokienzler/meshai/raw/master/rangemapper/img/gt1.png "Logo Title Text 1")

## Step 3 - Train the model and predict

Finally, the model can be used to predict range maps

![alt text](https://github.com/romeokienzler/meshai/raw/master/rangemapper/img/p1.png "Logo Title Text 1")

