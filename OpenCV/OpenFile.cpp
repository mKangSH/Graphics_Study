#include "opencv2/opencv.hpp"
//#include <GL/glut.h>
#include <GL/glui.h>
#include "OpenFileDialog.h"
#include "SaveFileDialog.h"

using namespace std;
using namespace cv;

/* Histogram class */
int histSize = 256;
bool uniform = true;
int hist_w = 512;
int hist_h = 400;
int bin_w = cvRound((double)(hist_w / histSize));
float range[] = { 0, 256 };
const float* histRange = { range };

void histMonitor(int id);
Mat bHistMonitor(Mat& histogramBuf);
Mat gHistMonitor(Mat& histogramBuf);
Mat rHistMonitor(Mat& histogramBuf);
Mat grayHistMonitor(Mat& histogramBuf);
/* Histogram class */

/* Edge class */
void edgeDetection(int id);
Mat sobelProcess(Mat& grayImage);
Mat laplacianProcess(Mat& grayImage);
Mat cannyProcess(Mat& grayImage);

double weightedValue1 = 0.5, weightedValue2 = 0.5, gamma = 0; // sobel edge 가중치 값
int lowThreshold = 1, Ratio = 3;
int sobelScale = 1, sobelDelta = 0;
int lapScale = 1, lapDelta = 0;
/* Edge class */

void morphologyProcess(int id);

int morphologyOp = 0;
int rectSize = 1;

void houghProcess(int id);
double rho = 1.0, minLineLength = 100, maxLineGap = 20;
int houghThreshold = 50;


int main_window;
String Filename;
Mat src;
int check1 = 1;
int rg1 = 0, rg2 = 0;
int editint = 10, edgeSelect = 4;
int histOn = 0, thresh = 0, maxVal;

void open(int id)
{
	OpenFileDialog* openFileDialog = new OpenFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
		cout << Filename << endl;
		src = imread(Filename, -1);
		imshow("src", src);
	}
}

void save(int id)
{
	SaveFileDialog* openFileDialog = new SaveFileDialog();
	if (openFileDialog->ShowDialog()) {
		Filename = openFileDialog->FileName;
		imwrite(Filename, src);
	}
}

void radiofunc(int id)
{
	Mat dst;

	if (rg1 != 5)
	{
		threshold(src, dst, thresh, maxVal, rg1); // Binary ~ TOZERO_INV: 0 ~ 4(integer)
		imshow("src", dst);
	}

	else
		imshow("src", src);
}

void myGlutIdle(void)
{
	GLUI_Master.sync_live_all();
	
	if (glutGetWindow() != main_window)
	{
		glutSetWindow(main_window);
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	GLUI* glui = GLUI_Master.create_glui("Menu", 0);
	main_window = glui->get_glut_window_id();

	GLUI_Button* button = glui->add_button("Open", 0, open);
	GLUI_Panel* main = glui->add_panel("", GLUI_PANEL_NONE);

	// button: FILE I/O
	//GLUI_Button *button1 = glui->add_button_to_panel(option1,"Save", 0, save);
	GLUI_Checkbox* cb1 = glui->add_checkbox("Histogram monitor", &histOn, 0, &histMonitor);	// check box: histogram

	GLUI_Rollout* option2 = glui->add_rollout_to_panel(main, "Threshold Process", FALSE);
	GLUI_Panel* temp2 = glui->add_panel_to_panel(option2, "", GLUI_PANEL_EMBOSSED);

	GLUI_RadioGroup* rg = glui->add_radiogroup_to_panel(temp2, &rg1, 0, radiofunc);
	glui->add_radiobutton_to_group(rg, "Binary");
	glui->add_radiobutton_to_group(rg, "Binary inverse");
	glui->add_radiobutton_to_group(rg, "Trunc");
	glui->add_radiobutton_to_group(rg, "To zero");
	glui->add_radiobutton_to_group(rg, "To zero inverse");
	glui->add_radiobutton_to_group(rg, "None");

	GLUI_Panel* bottom = glui->add_panel_to_panel(option2, NULL, GLUI_PANEL_NONE);

	GLUI_Spinner* edittext1 = glui->add_spinner_to_panel(bottom, "Thresh", GLUI_SPINNER_INT, &thresh);
	edittext1->set_int_limits(0, 255, GLUI_LIMIT_WRAP);

	glui->add_column_to_panel(bottom);

	GLUI_Spinner* edittext2 = glui->add_spinner_to_panel(bottom, "MaxValue", GLUI_SPINNER_INT, &maxVal);
	edittext2->set_int_limits(0, 255, GLUI_LIMIT_WRAP);

	glui->add_column_to_panel(main);

	GLUI_Rollout* option3 = glui->add_rollout_to_panel(main, "Edge Detection Process", FALSE);
	GLUI_Listbox* listbox1 = glui->add_listbox_to_panel(option3, "Edge", &edgeSelect, 0, &edgeDetection);
	listbox1->add_item(1, "Sobel");
	listbox1->add_item(2, "Laplacian");
	listbox1->add_item(3, "Canny");
	listbox1->add_item(4, "None");

	GLUI_Panel* temp3 = glui->add_panel_to_panel(option3, "Sobel", GLUI_PANEL_EMBOSSED);
	GLUI_EditText* edgeParameter1 = glui->add_edittext_to_panel(temp3, "Scale", GLUI_EDITTEXT_INT, &sobelScale);
	edgeParameter1->set_int_limits(0, 100, GLUI_LIMIT_NONE);
	GLUI_EditText* edgeParameter2 = glui->add_edittext_to_panel(temp3, "Delta", GLUI_EDITTEXT_INT, &sobelDelta);
	edgeParameter2->set_int_limits(0, 100, GLUI_LIMIT_NONE);

	GLUI_Panel* temp4 = glui->add_panel_to_panel(option3, "Laplacian", GLUI_PANEL_EMBOSSED);
	GLUI_EditText* edgeParameter3 = glui->add_edittext_to_panel(temp4, "Scale", GLUI_EDITTEXT_INT, &lapScale);
	edgeParameter3->set_int_limits(0, 100, GLUI_LIMIT_NONE);
	GLUI_EditText* edgeParameter4 = glui->add_edittext_to_panel(temp4, "Delta", GLUI_EDITTEXT_INT, &lapDelta);
	edgeParameter4->set_int_limits(0, 100, GLUI_LIMIT_NONE);

	GLUI_Panel* temp5 = glui->add_panel_to_panel(option3, "Canny", GLUI_PANEL_EMBOSSED);
	GLUI_EditText* edgeParameter5 = glui->add_edittext_to_panel(temp5, "Low Threshold", GLUI_EDITTEXT_INT, &lowThreshold);
	edgeParameter5->set_int_limits(0, 100, GLUI_LIMIT_NONE);
	GLUI_EditText* edgeParameter6 = glui->add_edittext_to_panel(temp5, "Ratio", GLUI_EDITTEXT_INT, &Ratio);
	edgeParameter6->set_int_limits(0, 100, GLUI_LIMIT_NONE);

	glui->add_column_to_panel(main);

	GLUI_Rollout* option4 = glui->add_rollout_to_panel(main, "Morphology Process", FALSE);
	GLUI_Listbox* listbox2 = glui->add_listbox_to_panel(option4, "operation", &morphologyOp, 0, &morphologyProcess);
	listbox2->add_item(2, "Open");
	listbox2->add_item(3, "Close");
	listbox2->add_item(4, "Gradient");
	listbox2->add_item(5, "Tophat");
	listbox2->add_item(6, "Blackhat");

	GLUI_Spinner* edittext3 = glui->add_spinner_to_panel(option4, "Morph Size", GLUI_SPINNER_INT, &rectSize);
	edittext3->set_int_limits(0, 25, GLUI_LIMIT_WRAP);

	glui->add_column_to_panel(main);

	GLUI_Rollout* option5 = glui->add_rollout_to_panel(main, "hough Process", FALSE);
	GLUI_Panel* temp6 = glui->add_panel_to_panel(option5, "", GLUI_PANEL_NONE);

	GLUI_Button* button2 = glui->add_button_to_panel(temp6, "Hough Line Percent", 0, houghProcess);

	GLUI_Spinner* edittext4 = glui->add_spinner_to_panel(option5, "Hough Threshold(all)", GLUI_SPINNER_INT, &houghThreshold);
	edittext4->set_int_limits(0, 100, GLUI_LIMIT_WRAP);

	GLUI_Panel* temp8 = glui->add_panel_to_panel(option5, "Hough Percent parameter", GLUI_PANEL_EMBOSSED);
	GLUI_Spinner* edittext5 = glui->add_spinner_to_panel(temp8, "Minimum line length", GLUI_SPINNER_FLOAT, &minLineLength);
	edittext5->set_int_limits(0, 200, GLUI_LIMIT_WRAP);
	GLUI_Spinner* edittext6 = glui->add_spinner_to_panel(temp8, "Max line gap", GLUI_SPINNER_FLOAT, &maxLineGap);
	edittext6->set_int_limits(0, 100, GLUI_LIMIT_WRAP);

	new GLUI_Button(glui, "Quit", 0, (GLUI_Update_CB)exit);

	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	glui->set_main_gfx_window(main_window);
	glutMainLoop();

	return EXIT_SUCCESS;
}

void houghProcess(int id)
{
	Mat dst, cdst;
	dst = src.clone();

	if (src.channels() == 3)
		cvtColor(dst, dst, COLOR_BGR2GRAY);

	Canny(dst, dst, 100, 200);

	cvtColor(dst, cdst, COLOR_GRAY2BGR);

	vector<Vec4i> lines;

	HoughLinesP(dst, lines, rho, CV_PI / 180, houghThreshold, minLineLength, maxLineGap);
	for (size_t i = 0; i < lines.size(); i++)
	{
		line(cdst, Point(lines[i][0], lines[i][1]), Point(lines[i][2], lines[i][3]), Scalar(0, 0, 255), 3, LINE_AA);
	}

	imshow("src", cdst);
}

void morphologyProcess(int id)
{
	Mat dst;
	dst = src.clone();

	Mat kernel = getStructuringElement(MORPH_RECT, Size(rectSize, rectSize));
	if (src.channels() == 3)
		cvtColor(dst, dst, COLOR_BGR2GRAY);

	morphologyEx(dst, dst, morphologyOp, kernel);

	imshow("src", dst);
}

void edgeDetection(int id)
{
	Mat dst;
	dst = src.clone();

	if (src.channels() == 3)
		cvtColor(dst, dst, COLOR_BGR2GRAY);

	switch (edgeSelect)
	{
	case 1:
		dst = sobelProcess(dst);
		break;
	case 2:
		dst = laplacianProcess(dst);
		break;
	case 3:
		dst = cannyProcess(dst);
		break;
	case 4:
		dst = src;
		break;
	}

	imshow("src", dst);
}

void histMonitor(int id)
{
	if (histOn == 1)
	{
		Mat grayHist, grayPlane, histImage, temp;
		vector<Mat> bgrgPlanes, bgrgHist;

		Mat srcClone = src.clone();

		if (srcClone.channels() == 1)
		{
			calcHist(&srcClone, 1, 0, Mat(), grayHist, 1, &histSize, &histRange, uniform, false);
			histImage = grayHistMonitor(grayHist);
		}

		else if (srcClone.channels() == 3)
		{
			split(srcClone, bgrgPlanes);
			cvtColor(srcClone, grayPlane, COLOR_BGR2GRAY);
			bgrgPlanes.push_back(grayPlane);

			for (int i = 0; i < 4; i++)
			{
				bgrgHist.push_back(Mat(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0)));
				calcHist(&bgrgPlanes[i], 1, 0, Mat(), bgrgHist[i], 1, &histSize, &histRange, uniform, false);
			}

			hconcat(bHistMonitor(bgrgHist[0]), gHistMonitor(bgrgHist[1]), histImage);
			hconcat(rHistMonitor(bgrgHist[2]), grayHistMonitor(bgrgHist[3]), temp);

			histImage.push_back(temp);
		}

		imshow("Histogram Monitor", histImage);
	}

	else
	{
		destroyWindow("Histogram Monitor");
	}
}

Mat bHistMonitor(Mat& histogramBuf)
{
	Mat histImageB(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	normalize(histogramBuf, histogramBuf, 0, histImageB.rows, NORM_MINMAX, -1, Mat());

	for (int i = 0; i < 255; i++)
	{
		line(histImageB, Point(bin_w * (i), hist_h), Point(bin_w * (i), hist_h - histogramBuf.at<float>(i)), Scalar(255, 0, 0));
	}

	return histImageB;
}

Mat gHistMonitor(Mat& histogramBuf)
{
	Mat histImageG(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	normalize(histogramBuf, histogramBuf, 0, histImageG.rows, NORM_MINMAX, -1, Mat());

	for (int i = 0; i < 255; i++)
	{
		line(histImageG, Point(bin_w * (i), hist_h), Point(bin_w * (i), hist_h - histogramBuf.at<float>(i)), Scalar(0, 255, 0));
	}

	return histImageG;
}

Mat rHistMonitor(Mat& histogramBuf)
{
	Mat histImageR(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	normalize(histogramBuf, histogramBuf, 0, histImageR.rows, NORM_MINMAX, -1, Mat());

	for (int i = 0; i < 255; i++)
	{
		line(histImageR, Point(bin_w * (i), hist_h), Point(bin_w * (i), hist_h - histogramBuf.at<float>(i)), Scalar(0, 0, 255));
	}

	return histImageR;
}

Mat grayHistMonitor(Mat& histogramBuf)
{
	Mat histImageGray(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

	normalize(histogramBuf, histogramBuf, 0, histImageGray.rows, NORM_MINMAX, -1, Mat());

	for (int i = 0; i < 255; i++)
	{
		line(histImageGray, Point(bin_w * (i), hist_h), Point(bin_w * (i), hist_h - histogramBuf.at<float>(i)), Scalar(128, 128, 128));
	}

	return histImageGray;
}

Mat sobelProcess(Mat& grayImage)
{
	Mat grad;
	Mat gradX, gradY, absGradX, absGradY;

	Sobel(grayImage, gradX, CV_16S, 1, 0, 3, sobelScale, sobelDelta, BORDER_DEFAULT);
	Sobel(grayImage, gradY, CV_16S, 0, 1, 3, sobelScale, sobelDelta, BORDER_DEFAULT);

	convertScaleAbs(gradX, absGradX);
	convertScaleAbs(gradY, absGradY);

	addWeighted(absGradX, weightedValue1, absGradY, weightedValue2, gamma, grad);

	return grad;
}

Mat laplacianProcess(Mat& grayImage)
{
	Mat dst, absDst;

	GaussianBlur(grayImage, grayImage, Size(3, 3), 0, 0, BORDER_DEFAULT);
	Laplacian(grayImage, dst, CV_16S, 3, lapScale, lapDelta, BORDER_DEFAULT);
	convertScaleAbs(dst, absDst);

	return absDst;
}

Mat cannyProcess(Mat& grayImage)
{
	Mat detectedEdge, dst;
	blur(grayImage, detectedEdge, Size(3, 3));
	Canny(detectedEdge, detectedEdge, lowThreshold, lowThreshold * Ratio, 3);
	dst = Scalar::all(0);
	grayImage.copyTo(dst, detectedEdge);

	return dst;
}