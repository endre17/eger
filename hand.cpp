//OpenCV Headers
#include<cv.h>
#include<highgui.h>
//Input-Output
#include<stdio.h>
//Blob Library Headers
#include<cvblob.h>
//Definitions
#define h 240
#define w 320
//NameSpaces
using namespace cvb;
using namespace std;

int blue_nmbr=0, minarea=50, maxarea=1000;
int lower_red[3] = {150, 50, 50};
int upper_red[3] = {180, 255, 255};
int lower_blue[3] = {100, 135, 135};
int upper_blue[3] = {130, 255, 255};
int lower_green[3] = {25, 50, 50};
int upper_green[3] = {75, 255, 255};

void detect_blob(IplImage* hsvframe, int* lower, int* upper, IplImage* threshold, IplImage* labelImg, CvBlobs& blobs, IplImage* frame){

    //inputkepbol(hsvframe) a megadott intervallumu szineket hagyva meg teszi az outputkepbe(threshold)
    cvInRangeS(hsvframe,cvScalar(lower[0],lower[1],lower[2]),cvScalar(upper[0],upper[1],upper[2]),threshold);
    //A threshold kep szurese
    cvSmooth(threshold,threshold,CV_MEDIAN,7,7);
    //A blob-ok megkeresese. (result=pixelek szama)
    unsigned int result=cvLabel(threshold,labelImg,blobs);
    //Blobok hozzaadasa kephez
    cvRenderBlobs(labelImg,blobs,frame,frame);
    //Adott meretnek kisebb, ill. nagyobb teruletet figyelmen kivul hagyjuk
    cvFilterByArea(blobs,minarea,maxarea);

}


int main()
{
        //Structure to get feed from CAM
        CvCapture* capture=cvCreateCameraCapture(0);

        if (!capture) {
                cerr<< "Error initializing capture" << endl;
                exit(1);
        }
        //Structure to hold blobs
        CvBlobs red_blobs, blue_blobs, green_blobs;
        //Windows
        cvNamedWindow("Live",CV_WINDOW_AUTOSIZE);
        //Image Variables
        IplImage *frame=cvCreateImage(cvSize(w,h),8,3);   //Original Image
        IplImage *hsvframe=cvCreateImage(cvSize(w,h),8,3);//Image in HSV color space
        IplImage *labelImg_red=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs
        IplImage *labelImg_blue=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs
        IplImage *labelImg_green=cvCreateImage(cvSize(w,h),IPL_DEPTH_LABEL,1);//Image Variable for blobs
        //IplImage *threshy=cvCreateImage(cvSize(w,h),8,1); //Threshold image of yellow color
        IplImage *threshr=cvCreateImage(cvSize(w,h),8,1); //Threshold image of red color
        IplImage *threshb=cvCreateImage(cvSize(w,h),8,1); //Threshold image of blue color
        IplImage *threshg=cvCreateImage(cvSize(w,h),8,1); //Threshold image of green color

        //Getting the screen information
        int screenx = 1920;
        int screeny = 1080;

        while(1)
        {
                blue_nmbr=0;
                //Getting the current frame
                IplImage *fram=cvQueryFrame(capture);
                //If failed to get break the loop
                if(!fram)
                break;
                //Resizing the capture
                cvResize(fram,frame,CV_INTER_LINEAR );
                //Flipping the frame
                cvFlip(frame,frame,1);
                //Changing the color space
                cvCvtColor(frame,hsvframe,CV_BGR2HSV);

/*              //Ezt a reszt valositja meg a detect_blob fgv, hogy ne kelljen tobbszor ugyanazt a fgvt meghivni.
                //Thresholding the frame for yellow , inputkepbol(hsvframe) a megadott intervallumu szineket hagyva meg teszi az outputkepbe(threshy)
                //cvInRangeS(hsvframe,cvScalar(160,135,135),cvScalar(180,255,255),threshr);
                cvInRangeS(hsvframe,cvScalar(150,50,50),cvScalar(180,255,255),threshr);
                cvInRangeS(hsvframe,cvScalar(100,135,135),cvScalar(130,255,255),threshb);
                cvInRangeS(hsvframe,cvScalar(25,50,50),cvScalar(75,255,255),threshg);


                //Filtering the frame
                cvSmooth(threshr,threshr,CV_MEDIAN,7,7);
                cvSmooth(threshb,threshb,CV_MEDIAN,7,7);
                cvSmooth(threshg,threshg,CV_MEDIAN,7,7);


                //Finding the blobs (result:pixelek szama)
                unsigned int red_result=cvLabel(threshr,labelImg_red,red_blobs);
                unsigned int blue_result=cvLabel(threshb,labelImg_blue,blue_blobs);
                unsigned int green_result=cvLabel(threshg,labelImg_green,green_blobs);


                //Rendering the blobs
                cvRenderBlobs(labelImg_red,red_blobs,frame,frame);
                cvRenderBlobs(labelImg_blue,blue_blobs,frame,frame);
                cvRenderBlobs(labelImg_green,green_blobs,frame,frame);

                //Filtering the blobs
                cvFilterByArea(red_blobs,60,500);
                cvFilterByArea(blue_blobs,minarea,maxarea);
                cvFilterByArea(green_blobs,60,500);
*/
                //Piros teruletek keresese
                detect_blob(hsvframe, lower_red, upper_red, threshr, labelImg_red, red_blobs, frame);

                //Kek teruletek keresese
                detect_blob(hsvframe, lower_blue, upper_blue, threshb, labelImg_blue, blue_blobs, frame);

                //Zold terulek keresese
                detect_blob(hsvframe, lower_green, upper_green, threshg, labelImg_green, green_blobs, frame);


                //Kek teruletekbol mennyi van adott kepkockan.
                for (CvBlobs::const_iterator it=blue_blobs.begin(); it!=blue_blobs.end(); ++it){
                    blue_nmbr++;
                }
                if(blue_nmbr){
                    cout<<blue_nmbr<<endl;
                }

/*
                //Koordinatakat adja meg a blobhoz(kurzor mozgatashoz jo)
                for (CvBlobs::const_iterator it=blue_blobs.begin(); it!=blue_blobs.end(); ++it)
                {
                        double moment10 = it->second->m10;
                        double moment01 = it->second->m01;
                        double area = it->second->area;
                        //Variable for holding position
                        int x1;
                        int y1;
                        //Calculating the current position
                        x1 = moment10/area;
                        y1 = moment01/area;
                        //Mapping to the screen coordinates
                        int x=(int)(x1*screenx/w);
                        int y=(int)(y1*screeny/h);
                        //Printing the position information
                        cout<<"X: "<<x<<" Y: "<<y<<endl;
                }
*/

                //Showing the images
                cvShowImage("Live",frame);
                //Escape Sequence
                char c=cvWaitKey(33);
                if(c==27)
                break;
        }
        //Cleanup
        cvReleaseCapture(&capture);
        cvDestroyAllWindows();

}
