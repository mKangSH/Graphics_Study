#include <opencv2/opencv.hpp>
#include <GL/glui.h>
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"

#define LAPLACIAN 100
using namespace std;
using namespace cv;

int main_window;
String openFilename, saveFilename;
Mat videoFrame;
int button = 0;
float videoFPS;
int videoWidth, videoHeight, videoLength;
int fmValue1, fmValue2, fmValue3, saveValue;
int freadflag = -1;
int waitTime, key = 4;
int frame;
int lapScale = 1, lapDelta = 20;

unique_ptr<SaveFileDialog> saveFileDialog;

GLUI* glui = GLUI_Master.create_glui("Menu", 0);

VideoCapture videoCapture;
VideoWriter videoWriter;

void open(int id);
void save(int id);
void idle(void);
void fileMode(int id);
void play(int id);
void printnSaveImg();
Mat laplacianProcess(Mat& grayImage);

GLUI_Checkbox *fileMode1, *fileMode2, *fileMode3, *fileMode4;


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	
	main_window = glui->get_glut_window_id();
	GLUI_Master.set_glutIdleFunc(idle);

	GLUI_Panel* main = glui->add_panel("", GLUI_PANEL_NONE);
	GLUI_Button* button = glui->add_button_to_panel(main, "Webcam", 0, open);
	glui->add_column_to_panel(main, 0);
	GLUI_Button* button2 = glui->add_button_to_panel(main, "Open", 1, open);
	
	
	GLUI_Panel* main2 = glui->add_panel("", GLUI_PANEL_NONE);
	GLUI_Button* button3 = glui->add_button_to_panel(main2, "ll", 0, play);
	glui->add_column_to_panel(main2, 0);
	GLUI_Button* button4 = glui->add_button_to_panel(main2, "l<", 1, play);
	glui->add_column_to_panel(main2, 0);
	GLUI_Button* button5 = glui->add_button_to_panel(main2, "<<", 2, play);
	glui->add_column_to_panel(main2, 0);
	GLUI_Button* button6 = glui->add_button_to_panel(main2, "<", 3, play);
	glui->add_column_to_panel(main2, 0);
	GLUI_Button* button7 = glui->add_button_to_panel(main2, ">", 4, play);
	glui->add_column_to_panel(main2, 0);
	GLUI_Button* button8 = glui->add_button_to_panel(main2, ">>", 5, play);
	glui->add_column_to_panel(main2, 0);
	GLUI_Button* button9 = glui->add_button_to_panel(main2, ">l", 6, play);

	glui->add_separator();

	GLUI_Panel* bottom = glui->add_panel("", GLUI_PANEL_NONE);
	fileMode1 = glui->add_checkbox_to_panel(bottom, "Laplacian", &fmValue1, 1, fileMode);
	glui->add_column_to_panel(bottom);
	fileMode2 = glui->add_checkbox_to_panel(bottom, "Color", &fmValue2, 2, fileMode);
	glui->add_column_to_panel(bottom);
	fileMode3 = glui->add_checkbox_to_panel(bottom, "Gray", &fmValue3, 3, fileMode);
	glui->add_column_to_panel(bottom);
	fileMode4 = glui->add_checkbox_to_panel(bottom, "Save", &saveValue, 0, save);

	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

	namedWindow("video", 1);

	glui->set_main_gfx_window(main_window);
	glutMainLoop();

	return EXIT_SUCCESS;
}

void open(int id)
{
	switch (id)
	{
		case 0:
			videoCapture.open(0);

			videoFPS = videoCapture.get(CAP_PROP_FPS);
			videoWidth = videoCapture.get(CAP_PROP_FRAME_WIDTH);
			videoHeight = videoCapture.get(CAP_PROP_FRAME_HEIGHT);
	
			frame = 0;
			waitTime = 1000 / videoFPS;
			break;

		case 1:
			OpenFileDialog * openFileDialog = new OpenFileDialog();
			if (openFileDialog->ShowDialog()) 
			{
				openFilename = openFileDialog->FileName;
				cout << openFilename << endl;
			}

			videoCapture.open(openFilename);
	
			videoFPS = videoCapture.get(CAP_PROP_FPS);
			videoWidth = videoCapture.get(CAP_PROP_FRAME_WIDTH);
			videoHeight = videoCapture.get(CAP_PROP_FRAME_HEIGHT);
			videoLength = videoCapture.get(CAP_PROP_FRAME_COUNT);

			frame = 0;
			waitTime = 1000 / videoFPS;
			break;
	}
}

void save(int id)
{
	printf("%d\n", saveValue);
	
	switch(saveValue)
	{
		case 0:
			videoWriter.release();
			break;

		case 1:
			saveFileDialog  = make_unique<SaveFileDialog>();

			if (saveFileDialog->ShowDialog()) 
			{
				saveFilename = saveFileDialog->FileName;
			}

			if (videoCapture.isOpened())
			{
				videoWriter.open(saveFilename, VideoWriter::fourcc('M', 'J', 'P', 'G'), videoFPS, Size(videoWidth, videoHeight), true);
			}

			break;
	}

}

void idle(void)
{
	if (videoCapture.isOpened())
	{
		switch (key) 
		{
			case 0:
				printnSaveImg();
				break;

			case 1:
				videoCapture.set(CAP_PROP_POS_FRAMES, frame);
				videoCapture >> videoFrame;
				printnSaveImg();
				break;

			case 2:
				if (frame != 0)
				{
					--frame;
					videoCapture.set(CAP_PROP_POS_FRAMES, frame);
					videoCapture >> videoFrame;
					printnSaveImg();
				}
				break;

			case 3:
				if (frame != 0)
				{
					--frame;
					videoCapture.set(CAP_PROP_POS_FRAMES, frame);
					videoCapture >> videoFrame;
					printnSaveImg();
				}
				break;

			case 4:
				if (frame != videoLength - 1)
				{
					++frame;
					videoCapture.set(CAP_PROP_POS_FRAMES, frame);
					videoCapture >> videoFrame;
					printnSaveImg();
				}
				break;

			case 5:
				if (frame != videoLength - 1)
				{
					++frame;
					videoCapture.set(CAP_PROP_POS_FRAMES, frame);
					videoCapture >> videoFrame;
					printnSaveImg();
				}
				break;

			case 6:
				videoCapture >> videoFrame;
				videoCapture.set(CAP_PROP_POS_FRAMES, frame);
				printnSaveImg();
				break;
		}

		waitKey(waitTime);
	}

	else
	{
		videoCapture.release();
	}
}

void fileMode(int id)
{
	switch (id)
	{
		case 1: fmValue1 = 1; fmValue2 = 0; fmValue3 = 0; freadflag = LAPLACIAN; break;
		case 2: fmValue1 = 0; fmValue2 = 1; fmValue3 = 0; freadflag = IMREAD_COLOR; break;
		case 3: fmValue1 = 0; fmValue2 = 0; fmValue3 = 1; freadflag = IMREAD_GRAYSCALE; break;
	}
	
	fileMode1->set_int_val(fmValue1);
	fileMode2->set_int_val(fmValue2);
	fileMode3->set_int_val(fmValue3);
	glui->sync_live();
}

void play(int id)
{
	key = id;
	switch (id)
	{
		case 0:
			waitTime = 1000 / videoFPS;
			break;

		case 1:
			waitTime = 1000 / videoFPS;
			frame = 0;
			break;

		case 2:
			waitTime = (1000 / videoFPS) / 3;
			break;

		case 3:
			waitTime = 1000 / videoFPS;
			break;

		case 4:
			waitTime = 1000 / videoFPS;
			break;

		case 5:
			waitTime = (1000 / videoFPS) / 3;
			break;

		case 6:
			waitTime = 1000 / videoFPS;
			frame = videoLength - 1;
			break;
	}
}

void printnSaveImg()
{
	if (!videoFrame.empty())
	{
		Mat gray;
		Mat dst = videoFrame.clone();

		if (freadflag == IMREAD_GRAYSCALE)
		{
			cvtColor(videoFrame, gray, COLOR_BGR2GRAY);
			cvtColor(gray, dst, COLOR_GRAY2BGR);
		}

		else if (freadflag == LAPLACIAN)
		{
			cvtColor(videoFrame, gray, COLOR_BGR2GRAY);
			gray = laplacianProcess(gray);
			cvtColor(gray, dst, COLOR_GRAY2BGR);
		}

		imshow("video", dst);

		if (videoWriter.isOpened())
		{
			videoWriter << dst;
		}
	}
}

Mat laplacianProcess(Mat& grayImage)
{
	Mat dst, absDst;

	GaussianBlur(grayImage, grayImage, Size(3, 3), 0, 0, BORDER_DEFAULT);
	Laplacian(grayImage, dst, CV_16S, 3, lapScale, lapDelta, BORDER_DEFAULT);
	convertScaleAbs(dst, absDst);

	return absDst;
}