/* Copyright (C) 2012-2017 Ultraleap Limited. All rights reserved.
 *
 * Use of this code is subject to the terms of the Ultraleap SDK agreement
 * available at https://central.leapmotion.com/agreements/SdkAgreement unless
 * Ultraleap has signed a separate license agreement with you or your
 * organisation.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "LeapC.h"
#include "ExampleConnection.h"

int64_t lastFrameID = 0; //The last frame received

int main(int argc, char** argv) {
	
  FILE *fptr;

  OpenConnection();
  while(!IsConnected)
    millisleep(100); //wait a bit to let the connection complete

  printf("Connected.");
  LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
  if(deviceProps)
    printf("Using device %s.\n", deviceProps->serial);
  
  long i;
  bool frameFound = false;
  for (;;){
    LEAP_TRACKING_EVENT *frame = GetFrame();
    if(frame && (frame->tracking_frame_id > lastFrameID)){
		frameFound = true;
      lastFrameID = frame->tracking_frame_id;
      printf("Frame %lli with %i hands.\n", (long long int)frame->tracking_frame_id, frame->nHands);
      for(uint32_t h = 0; h < frame->nHands; h++){
		char jsonResult[512];
		char currentJSONEntry[512];
		
        LEAP_HAND* hand = &frame->pHands[h];
		
		strcpy(jsonResult, "{");
		//Distal ends of bones for each digit
        for(int f = 0; f < 5; f++){
          LEAP_DIGIT finger = hand->digits[f];
          for(int b = 0; b < 4; b++){
            LEAP_BONE bone = finger.bones[b];
			sprintf(currentJSONEntry, "\"finger_%d_bone_%d_x\":%f,", f,b, bone.next_joint.x);
			strcat(jsonResult, currentJSONEntry);
			sprintf(currentJSONEntry, "\"finger_%d_bone_%d_y\":%f,", f,b, bone.next_joint.y);
			strcat(jsonResult, currentJSONEntry);
			sprintf(currentJSONEntry, "\"finger_%d_bone_%d_z\":%f,", f,b, bone.next_joint.z);
			strcat(jsonResult, currentJSONEntry);
          }
        }
		
		sprintf(currentJSONEntry, "\"palm_x\":%f,", hand->palm.position.x);
		strcat(jsonResult, currentJSONEntry);
		sprintf(currentJSONEntry, "\"palm_y\":%f,", hand->palm.position.y);
		strcat(jsonResult, currentJSONEntry);
		sprintf(currentJSONEntry, "\"palm_z\":%f,", hand->palm.position.z);
		strcat(jsonResult, currentJSONEntry);
		
		sprintf(currentJSONEntry, "\"ellbow_x\":%f,", hand->arm.prev_joint.x);
		strcat(jsonResult, currentJSONEntry);
		sprintf(currentJSONEntry, "\"ellbow_y\":%f,", hand->arm.prev_joint.y);
		strcat(jsonResult, currentJSONEntry);
		sprintf(currentJSONEntry, "\"ellbow_z\":%f", hand->arm.prev_joint.z);
		strcat(jsonResult, currentJSONEntry);
		
		
		strcat(jsonResult, "}");
		fptr = fopen("leapjson.dat","w");

		if(fptr == NULL)
		{
			printf("Error!");   
			exit(1);             
		}

		fprintf(fptr, jsonResult);
		fclose(fptr);
      }
    }
	if(frameFound){
		millisleep(100);
	}
  } //ctrl-c to exit
  return 0;
}
//End-of-Sample
