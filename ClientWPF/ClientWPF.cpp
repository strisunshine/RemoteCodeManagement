///////////////////////////////////////////////////////////////////////
// ClientWPF.cpp - C++\CLI implementation of WPF Application         //
//          -  CSE 687 Project #4, 2015                              //
// Ver 1.0                                                           //
// Language:    Visual C++, Visual Studio 2013                       //
// platform:    Windows 7                                            //
// Application: CSE687 2015 - Project 4 - Remote Code Management     //
// Author:      Wenting Wang, wwang34@syr.edu                        //
// Source:      Jim Fawcett, jfawcett@twcny.rr.com                   //
///////////////////////////////////////////////////////////////////////

#include<sstream>
#include<chrono>

#include "ClientWPF.h"
using namespace CppCliWindows;

ClientWPF::ClientWPF(String^ arg1, String^ arg2)
{
	// set up Client via object factory
	ObjectFactory* pObjFact = new ObjectFactory;
	std::string portstr = toStdString(arg1);
	std::string loc = toStdString(arg2);
	std::stringstream ss;
	ss << portstr;
	size_t port;
	ss >> port;
	portstr_ = toSystemString(portstr);
	pClient = pObjFact->createClient(port, loc);
	pSender = pClient->getSender();
	pReceiver = pClient->getReceiver();
	delete pObjFact;

	// Client's receive thread
	isRecvThreadEnded = false;
	recvThread = gcnew Thread(gcnew ThreadStart(this, &ClientWPF::getMessage));
	recvThread->Start();

	// set event handlers for window UI itself
	this->Loaded +=
		gcnew System::Windows::RoutedEventHandler(this, &ClientWPF::OnLoaded);
	this->Closing +=
		gcnew CancelEventHandler(this, &ClientWPF::Unloading);

	// set event handlers for every view
	setEventHandlersforFileSearch();
	setEventHandlersforFileUpload();
	setEventHandlersforTextSearch();

	// set Window properties
	this->Title = "WPF C++/CLI (port:" + arg1 + ", loc: " + arg2 + ")";
	this->Width = 800;
	this->Height = 600;

	// attach dock panel to Window
	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);

	// setup Window controls and views
	setUpTabControl();
	setUpStatusBar();
	setupFileSearchView();
	setUpFileUploadView();
	setUpTextSearchView();
}

ClientWPF::~ClientWPF()
{
	delete pClient;
	delete pSender;
	delete pReceiver;
}

//----------< set Event Handlers for FileSearch View >-----------------------------------------
void ClientWPF::setEventHandlersforFileSearch(){
	// event handlers for FileSearchView
	hFileSearchButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::sendFileSearchRequest);
	hFileSearchListBox->SelectionChanged += gcnew SelectionChangedEventHandler(this, &ClientWPF::changeDownloadBttnVisibility);
	hDownloadButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::sendFileDownloadRequest);
	hFileSearchClearButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::clearFileSearchView);
	hSendButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::sendMessage);
	hClearButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::clear);
}

//----------< set Event Handlers for FileUpload View >-----------------------------------------
void ClientWPF::setEventHandlersforFileUpload(){
	// event handlers for FileUploadView
	hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::browseForFolder);
	hFileUploadButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::sendFileUploadRequest);
	hFileUploadListBox->SelectionChanged += gcnew SelectionChangedEventHandler(this, &ClientWPF::changeUploadBttnVisibility);
	hFileUploadClearButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::clearFileUploadView);
}

//----------< set Event Handlers for TextSearch View >-----------------------------------------
void ClientWPF::setEventHandlersforTextSearch(){
	// event handlers for TextSearchView
	hTextSearchButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::sendTextSearchRequest);
	hTextSearchClearButton->Click += gcnew RoutedEventHandler(this, &ClientWPF::clearTextSearchView);
}

//----------< set up the status bar at the bottoms of all tabs >---------------------------------
void ClientWPF::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "very important messages will appear here";
	//status->FontWeight = FontWeights::Bold;
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

void ClientWPF::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hFileSearchTab->Header = "File Search";
	hSendMessageTab->Header = "Send Message";
	hFileUploadTab->Header = "File Upload";
	hTextSearchTab->Header = "Text Search";
	hTabControl->Items->Add(hFileSearchTab);
	//hTabControl->Items->Add(hSendMessageTab);
	hTabControl->Items->Add(hFileUploadTab);
	hTabControl->Items->Add(hTextSearchTab);
}

void ClientWPF::setupFileSearchView(){
	Console::Write("\n  setting up FileSearch view");
	hFileSearchGrid->Margin = Thickness(10);
	hFileSearchTab->Content = hFileSearchGrid;
	setFileSearchOptions1();
	setFileSearchOptions2();
	setFileSearchControls1();
	setFileSearchResults();
	setFileSearchControls2();
};

void ClientWPF::setFileSearchOptions1(){
	//The first row enable the input of server port number
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(30);
	hFileSearchGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder11 = gcnew Border();
	hBorder11->Width = 310;
	hBorder11->Height = 20;
	hBorder11->Margin = Thickness(10, 0, 0, 0);
	hBorder11->BorderThickness = Thickness(0);
	hServerPortTag->Text = "Server Port (Please provide an integer port number):";
	hServerPortTag->TextAlignment = TextAlignment::Left;
	hBorder11->Child = hServerPortTag;
	Border^ hBorder12 = gcnew Border();
	hBorder12->Width = 120;
	hBorder12->Height = 30;
	hBorder12->BorderThickness = Thickness(1);
	hBorder12->Child = hFileSearchServerPortTextBox;
	fileSearchParamStackPanel1->Children->Add(hBorder11);
	fileSearchParamStackPanel1->Children->Add(hBorder12);
	fileSearchParamStackPanel1->Orientation = Orientation::Horizontal;
	fileSearchParamStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;
	hFileSearchGrid->SetRow(fileSearchParamStackPanel1, 0);
	hFileSearchGrid->Children->Add(fileSearchParamStackPanel1);
	//The second row enable the input of server relative path
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(30);
	hFileSearchGrid->RowDefinitions->Add(hRow2Def);
	Border^ hBorder21 = gcnew Border();
	hBorder21->Width = 310;
	hBorder21->Height = 20;
	hBorder21->Margin = Thickness(10, 0, 0, 0);
	hBorder21->BorderThickness = Thickness(0);
	hServerPathTag->Text = "Relative Path on Server (Current server path is \".\"):";
	hServerPathTag->TextAlignment = TextAlignment::Left;
	hBorder21->Child = hServerPathTag;
	Border^ hBorder22 = gcnew Border();
	hBorder22->Width = 180;
	hBorder22->Height = 30;
	hBorder22->BorderThickness = Thickness(1);
	hBorder22->Child = hServerPathTextBox;
	fileSearchParamStackPanel2->Children->Add(hBorder21);
	fileSearchParamStackPanel2->Children->Add(hBorder22);
	fileSearchParamStackPanel2->Orientation = Orientation::Horizontal;
	fileSearchParamStackPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

	hFileSearchGrid->SetRow(fileSearchParamStackPanel2, 1);
	hFileSearchGrid->Children->Add(fileSearchParamStackPanel2);
}
void ClientWPF::setFileSearchOptions2(){
	//The third row enable the input of file patterns to search
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(30);
	hFileSearchGrid->RowDefinitions->Add(hRow3Def);

	Border^ hBorder31 = gcnew Border();
	hBorder31->Width = 310;
	hBorder31->Height = 20;
	hBorder31->Margin = Thickness(10, 0, 0, 0);
	hBorder31->BorderThickness = Thickness(0);
	hFilePatternsTag->Text = "File Patterns (Separate patterns by space):";
	hFilePatternsTag->TextAlignment = TextAlignment::Left;
	hBorder31->Child = hFilePatternsTag;
	Border^ hBorder32 = gcnew Border();
	hBorder32->Width = 380;
	hBorder32->Height = 30;
	hBorder32->BorderThickness = Thickness(1);
	hBorder32->Child = hFilePatternsTextBox;

	fileSearchParamStackPanel3->Children->Add(hBorder31);
	fileSearchParamStackPanel3->Children->Add(hBorder32);
	fileSearchParamStackPanel3->Orientation = Orientation::Horizontal;
	fileSearchParamStackPanel3->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

	hFileSearchGrid->SetRow(fileSearchParamStackPanel3, 2);
	hFileSearchGrid->Children->Add(fileSearchParamStackPanel3);
}

void ClientWPF::setFileSearchControls1(){
	//The fourth row displays the buttons for user to explore the xml string or download files
	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow4Def->Height = GridLength(55);
	hFileSearchGrid->RowDefinitions->Add(hRow4Def);

	Border^ hBorder41 = gcnew Border();
	hBorder41->Width = 240;
	hBorder41->Height = 35;
	hBorder41->Margin = Thickness(85, 10, 15, 10);
	hBorder41->BorderThickness = Thickness(1);
	hFileSearchButton->Content = "File List (Download provided)";
	hBorder41->Child = hFileSearchButton;

	hFileSearchXMLCheckBox->Content = "Display as XML string";
	hFileSearchXMLCheckBox->Height = 20;
	hFileSearchXMLCheckBox->Margin = Thickness(30, 10, 0, 10);

	fileSearchButtonStackPanel->Children->Add(hBorder41);
	fileSearchButtonStackPanel->Children->Add(hFileSearchXMLCheckBox);
	fileSearchButtonStackPanel->Orientation = Orientation::Horizontal;
	fileSearchButtonStackPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

	hFileSearchGrid->SetRow(fileSearchButtonStackPanel, 3);
	hFileSearchGrid->Children->Add(fileSearchButtonStackPanel);
}
void ClientWPF::setFileSearchResults(){
	//The fifth row display the file search result (xml string or a list of files responding to button click)

	//hRow5Def->Height = GridLength(280);
	hFileSearchGrid->RowDefinitions->Add(hRow5Def);

	hFileSearchListBox->BorderThickness = Thickness(1);
	Border^ hXMLTextBlockBorder = gcnew Border();
	hXMLTextBlockBorder->BorderThickness = Thickness(1);
	hXMLTextBlockBorder->BorderBrush = Brushes::Black;
	hFileSearchXMLTextBlock->TextWrapping = TextWrapping::Wrap;
	hXMLTextBlockBorder->Child = hFileSearchXMLTextBlock;
	hFileSearchXMLScrollViewer->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hFileSearchXMLScrollViewer->Content = hXMLTextBlockBorder;

	hFileSearchGrid->SetRow(hFileSearchListBox, 4);
	hFileSearchGrid->Children->Add(hFileSearchListBox);
}

void ClientWPF::setFileSearchControls2(){
	// The sixth row display the button for file download when a file is selected in the file list view
	// as well as summary for file search and results of file download
	RowDefinition^ hRow6Def = gcnew RowDefinition();
	hRow6Def->Height = GridLength(80);
	hFileSearchGrid->RowDefinitions->Add(hRow6Def);


	hFileSearchStatusBar->Items->Add(hFileSearchStatusBarItem);
	hFileSearchStatus->Text = "File search summary and file download results will appear here: \n";
	hFileSearchStatus->TextWrapping = TextWrapping::Wrap;
	hFileSearchStatusScrollViewer->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hFileSearchStatusScrollViewer->Content = hFileSearchStatus;
	hFileSearchStatusScrollViewer->Height = 80;
	hFileSearchStatusScrollViewer->Width = 495;
	hFileSearchStatusScrollViewer->Padding = Thickness(5, 0, 0, 0);
	hFileSearchStatusBarItem->Content = hFileSearchStatusScrollViewer;
	fileDownloadButtonStackPanel->Children->Add(hFileSearchStatusBar);

	Border^ hBorder61 = gcnew Border();
	hBorder61->BorderThickness = Thickness(1);
	hBorder61->Width = 120;
	hBorder61->Height = 30;
	hBorder61->Margin = Thickness(5, 10, 5, 0);
	hFileSearchClearButton->Content = "Clear";
	hBorder61->Child = hFileSearchClearButton;
	fileDownloadButtonStackPanel->Children->Add(hBorder61);

	Border^ hBorder62 = gcnew Border();
	hBorder62->BorderThickness = Thickness(1);
	hBorder62->Width = 120;
	hBorder62->Height = 30;
	hBorder62->Margin = Thickness(0, 10, 5, 0);
	hDownloadButton->Content = "Download";
	hBorder62->Child = hDownloadButton;
	hDownloadButton->Visibility = System::Windows::Visibility::Hidden;
	fileDownloadButtonStackPanel->Children->Add(hBorder62);

	fileDownloadButtonStackPanel->Orientation = Orientation::Horizontal;
	fileDownloadButtonStackPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

	hFileSearchGrid->SetRow(fileDownloadButtonStackPanel, 5);
	hFileSearchGrid->Children->Add(fileDownloadButtonStackPanel);
}
void ClientWPF::setTextBlockProperties()
{
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hSendMessageGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hTextBlock1;
	hTextBlock1->Padding = Thickness(15);
	hTextBlock1->Text = "";
	hTextBlock1->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hTextBlock1->FontWeight = FontWeights::Bold;
	hTextBlock1->FontSize = 16;
	hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewer1->Content = hBorder1;
	hSendMessageGrid->SetRow(hScrollViewer1, 0);
	hSendMessageGrid->Children->Add(hScrollViewer1);
}

void ClientWPF::setButtonsProperties()
{
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	hSendMessageGrid->RowDefinitions->Add(hRow2Def);
	hSendButton->Content = "Send Message";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hClearButton->Content = "Clear";
	hBorder2->Child = hSendButton;
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hClearButton;
	hStackPanel1->Children->Add(hBorder2);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 10;
	hStackPanel1->Children->Add(hSpacer);
	hStackPanel1->Children->Add(hBorder3);
	hStackPanel1->Orientation = Orientation::Horizontal;
	hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hSendMessageGrid->SetRow(hStackPanel1, 1);
	hSendMessageGrid->Children->Add(hStackPanel1);
}

void ClientWPF::setUpSendMessageView()
{
	Console::Write("\n  setting up sendMessage view");
	hSendMessageGrid->Margin = Thickness(20);
	hSendMessageTab->Content = hSendMessageGrid;

	setTextBlockProperties();
	setButtonsProperties();
}

std::string ClientWPF::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}

void ClientWPF::sendMessage(Object^ obj, RoutedEventArgs^ args)
{
	//pSendr_->postMessage(toStdString(msgText));
	Console::Write("\n  sent message");
	hStatus->Text = "Sent message";
}

//----< Search files based on user input after uses clicks on FileSearch Button >------
void ClientWPF::sendFileSearchRequest(Object^ obj, RoutedEventArgs^ args){
	hDownloadButton->Visibility = System::Windows::Visibility::Hidden;
	Message ms;
	String^ sserverport = hFileSearchServerPortTextBox->Text;
	std::string dstAddress = "::1@" + toStdString(sserverport);
	String^ sserverpath = hServerPathTextBox->Text;
	if (sserverpath == "")  sserverpath = ".";
	std::string serverpath = toStdString(sserverpath);
	String^ spatterns = hFilePatternsTextBox->Text;
	if (spatterns == "")  spatterns = "*.*";
	std::string patterns = toStdString(spatterns);
	bool ifasxml = hFileSearchXMLCheckBox->IsChecked.Value;
	String^ sasxml = ifasxml.ToString();
	std::string asxml = toStdString(sasxml);
	ms.configureFileSearchMsg("::1@" + toStdString(portstr_), dstAddress, asxml, patterns, serverpath);
	pClient->makeRequest(ms);
	Console::Write("\n  Sent file search request");
	hStatus->Text = "Sent file search request";
}

//----< Download the file after uses clicks on Download Button >------------------------------
void ClientWPF::sendFileDownloadRequest(Object^ obj, RoutedEventArgs^ args){
	Message ms;
	String^ sserverport = hFileSearchServerPortTextBox->Text;
	std::string dstAddress = "::1@" + toStdString(sserverport);
	String^ sfilename = System::IO::Path::GetFileName(hFileSearchListBox->SelectedItem->ToString());
	std::string filename = toStdString(sfilename);
	String^ spath = System::IO::Path::GetDirectoryName(hFileSearchListBox->SelectedItem->ToString());
	std::string serverrelativepath = toStdString(spath);
	ms.configureFileDownloadMsg(filename, "::1@" + toStdString(portstr_), dstAddress, serverrelativepath);
	pClient->makeRequest(ms);
	Console::Write("\n  Sent file download request");
	hStatus->Text = "Sent file download request";
};

//----< Clear the whole FileSearchView after uses clicks on Clear Button >--------------------
void ClientWPF::clearFileSearchView(Object^ obj, RoutedEventArgs^ args){
	hFileSearchServerPortTextBox->Clear();
	hServerPathTextBox->Clear();
	hFilePatternsTextBox->Clear();
	hFileSearchXMLCheckBox->IsChecked = false;
	hFileSearchListBox->Items->Clear();
	hFileSearchXMLTextBlock->Text = "";
	hFileSearchStatus->Text = "File search summary and file download results will appear here: \n";
	Console::Write("\n  Cleared FileSearch view");
	hStatus->Text = "Cleared FileSearch view";
}

//-----------< Upload the file selected after uses clicks on Upload Button >------------------
void ClientWPF::sendFileUploadRequest(Object^ obj, RoutedEventArgs^ args){
	Message ms;
	String^ sserverport = hUploadServerPortTextBox->Text;
	std::string dstAddress = "::1@" + toStdString(sserverport);
	String^ sfilename = System::IO::Path::GetFileName(hFileUploadListBox->SelectedItem->ToString());
	std::string filename = toStdString(sfilename);
	String^ spath = System::IO::Path::GetDirectoryName(hFileUploadListBox->SelectedItem->ToString());
	std::string path = toStdString(spath);
	ms.configureFileUploadMsg(filename, "::1@" + toStdString(portstr_), dstAddress, path);
	pClient->makeRequest(ms);	
	Console::Write("\n  Sent file upload request");
	hStatus->Text = "Sent file upload request";
}

//-----------< Only show Download Button when a file is selected >-----------------------------
void ClientWPF::changeDownloadBttnVisibility(Object^ obj, SelectionChangedEventArgs ^ args){
	if (hFileSearchListBox->SelectedIndex == -1 || hFileSearchListBox->SelectedItem->ToString()->IndexOf("\n") == 0) hDownloadButton->Visibility = System::Windows::Visibility::Hidden;
	else hDownloadButton->Visibility = System::Windows::Visibility::Visible;
}

//-----------< Only show Upload Button when a file is selected >-------------------------------
void ClientWPF::changeUploadBttnVisibility(Object^ obj, SelectionChangedEventArgs ^ args){
	if (hFileUploadListBox->SelectedIndex == -1 || hFileUploadListBox->SelectedItem->ToString()->Substring(0, 2) == L"<>") hFileUploadButton->Visibility = System::Windows::Visibility::Hidden;
	else hFileUploadButton->Visibility = System::Windows::Visibility::Visible;
}

//-----------< Clear the whole FileUploadView after user clicks on the Clear button >-----------
void ClientWPF::clearFileUploadView(Object^ obj, RoutedEventArgs ^ args){
	hUploadServerPortTextBox->Clear();
	hFileUploadListBox->Items->Clear();
	hFileUploadStatus->Text = "Upload replies will appear here: \n";;
	Console::Write("\n  Cleared FileUpload view");
	hStatus->Text = "Cleared FileUpload view";
}

//-----------< Send text search request after user clicks on the Search button >----------------
void ClientWPF::sendTextSearchRequest(Object^ obj, RoutedEventArgs ^ args){
	Message ms;
	String^ sserver1port = hTextSearchServer1PortTextBox->Text;
	std::string dst1Address = "::1@" + toStdString(sserver1port);
	String^ sserver1threads = hTextSearchServer1ThreadsComboBox->Text;
	std::string server1threads = toStdString(sserver1threads);

	String^ sserver2port = hTextSearchServer2PortTextBox->Text;
	std::string dst2Address = "::1@" + toStdString(sserver2port);
	String^ sserver2threads = hTextSearchServer2ThreadsComboBox->Text;
	std::string server2threads = toStdString(sserver2threads);

	String^ spatterns = hTextSearchFilePatternsTextBox->Text;
	if (spatterns == "")  spatterns = "*.*";
	std::string patterns = toStdString(spatterns);

	String^ ssearchtext = hTextSearchTextTextBox->Text;
	std::string searchtext = toStdString(ssearchtext);

	bool ifasxml = hTextSearchXMLCheckBox->IsChecked.Value;
	String^ sasxml = ifasxml.ToString();
	std::string asxml = toStdString(sasxml);

	if (sserver1port != ""){
		ms.configureTextSearchMsg("::1@" + toStdString(portstr_), dst1Address, asxml, patterns, searchtext, server1threads);
		pClient->makeRequest(ms);
		Console::Write("\n  Sent text search request");
		hStatus->Text = "Sent text search request";
	}
	else{
		Console::Write("\n  Server 1 port is not specified");
	}
	if (sserver2port != ""){
		ms.configureTextSearchMsg("::1@" + toStdString(portstr_), dst2Address, asxml, patterns, searchtext, server2threads);
		pClient->makeRequest(ms);
		Console::Write("\n  Sent text search request");
		hStatus->Text = "Sent text search request";
	}
	else{
		Console::Write("\n  Server 2 port is not specified");
	}
}

//-----------< Clear the whole TextSearchView after user clicks on the Clear button >-----------
void ClientWPF::clearTextSearchView(Object^ obj, RoutedEventArgs ^ args){
	hTextSearchServer1PortTextBox->Clear();
	hTextSearchServer1ThreadsComboBox->Text = "";
	hTextSearchServer2PortTextBox->Clear();
	hTextSearchServer2ThreadsComboBox->Text = "";
	hTextSearchFilePatternsTextBox->Clear();
	hTextSearchXMLCheckBox->IsChecked = false;
	hTextSearchTextTextBox->Clear();
	hTextSearchListBox->Items->Clear();
	hTextSearchXMLTextBlock->Text = "";
	hTextSearchStatus->Text = "Text search performance inforamtion will appear here : \n";
	Console::Write("\n  Cleared TextSearch view");
	hStatus->Text = "Cleared TextSearch view";
}

String^ ClientWPF::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}

void ClientWPF::addText(String^ msg)
{
	hTextBlock1->Text += msg + "\n\n";
}

//---------< Display file upload result on FileUploadView >------------
void ClientWPF::showFileUploadReplies(String^ msg)
{
	hFileUploadStatus->Text += msg + "\n";
}

//---------< Display file search result on FileSearchView >-------------
void ClientWPF::showFileSearchResult(String^ sMsg)
{
	hFileSearchGrid->Children->RemoveAt(4);
	hFileSearchGrid->SetRow(hFileSearchListBox, 4);
	hFileSearchGrid->Children->Insert(4, hFileSearchListBox);

	array<String^>^ msgs = sMsg->Split('\n');
	hFileSearchListBox->Items->Clear();
	for (int i = 0; i < msgs->Length-1; ++i){
		hFileSearchListBox->Items->Add(msgs[i]);
	}
}

//---------< Display file search result as XML on FileSearchView >------
void ClientWPF::showFileSearchResultXML(String^ sMsg)
{
	hFileSearchGrid->Children->RemoveAt(4);
	hFileSearchGrid->SetRow(hFileSearchXMLScrollViewer, 4);
	hFileSearchGrid->Children->Insert(4, hFileSearchXMLScrollViewer);

	hFileSearchXMLTextBlock->Text = sMsg;
}

//---------< Display file search summary on FileSearchView >------
void ClientWPF::showFileSearchSummary(String^ sMsg)
{
	hFileSearchStatus->Text += sMsg + "\n";
}

//---------< Display text search result on FileSearchView >-------------
void ClientWPF::showTextSearchResult(String^ sMsg)
{
	hTextSearchGrid->Children->RemoveAt(5);
	hTextSearchGrid->SetRow(hTextSearchListBox, 5);
	hTextSearchGrid->Children->Insert(5, hTextSearchListBox);

	array<String^>^ msgs = sMsg->Split('\n');
	//hTextSearchListBox->Items->Clear();
	for (int i = 0; i < msgs->Length - 1; ++i){
		hTextSearchListBox->Items->Add(msgs[i]);
	}
}

//---------< Display text search result as XML on TextSearchView >------
void ClientWPF::showTextSearchResultXML(String^ sMsg)
{
	hTextSearchGrid->Children->RemoveAt(5);
	hTextSearchGrid->SetRow(hTextSearchXMLScrollViewer, 5);
	hTextSearchGrid->Children->Insert(5, hTextSearchXMLScrollViewer);

	hTextSearchXMLTextBlock->Text += sMsg;
}

//---------< Show text search summary on TextSearchView >------
void ClientWPF::showTextSearchSummary(String^ sMsg)
{
	hTextSearchStatus->Text += sMsg + "\n";
}

//---------< Display file download result on TextSearchView >------------
void ClientWPF::showFileDownloadResult(String^ msg){
	hFileSearchStatus->Text += msg + "\n";
}

//---------< Display text on the status bar at the bottom >--------------
void ClientWPF::addStatusText(String^ msg){
	hStatus->Text += msg + "\n";
};

void ClientWPF::getMessage()
{
	// recvThread runs this function

	while (!isRecvThreadEnded)
	{
		std::cout << "\n  receive thread calling getMessage()";
		Message ms = pReceiver->deQSingleRequest();

		// Dispatch messages according to commands
		if (ms.getCommand() == "FILE_UPLOAD_REPLY"){
			fileUploadReplyDispatch(ms);
		}
		else if (ms.getCommand() == "FILE_SEARCH_REPLY"){
			fileSearchReplyDispatch(ms);
		}
		else if (ms.getCommand() == "TEXT_SEARCH_REPLY"){
			textSearchReplyDispatch(ms);
		}
		else if (ms.getCommand() == "FILE_DOWNLOAD_REPLY"){
			fileDownloadReplyDispatch(ms);
		}
		else if (ms.getCommand() == "QUIT_REPLY"){
			quitReplyDispatch(ms);
			break;
		}
	}
}

// a set of dispatchers for messages the client receivers
void ClientWPF::fileUploadReplyDispatch(Message& ms){
	std::string result, servernumber, path;
	result = ms.getAttributes()["result"];
	servernumber = ms.getAttributes()["servernumber"];
	path = ms.getAttributes()["path"];

	// display the server's reply of file upload on client's UI
	std::string uploadresult;
	if (result == "success") uploadresult = ms.getFiletoUpload() + " has been successfully uploaded to " + servernumber + " under " + path;
	else uploadresult = "The upload of " + ms.getFiletoUpload() + " to " + servernumber + " has failed";
	unsigned long long serverProcessTime = std::chrono::duration_cast<std::chrono::milliseconds>(ms.getServerSenttime() - ms.getServerRecvtime()).count();
	unsigned long long totalResponseTime = std::chrono::duration_cast<std::chrono::milliseconds>(ms.getClientRecvtime() - ms.getClientSenttime()).count();
	String^ sMsg = toSystemString(uploadresult);
	sMsg += ".  Server processing time: " + serverProcessTime.ToString() + ",  Total response time: " + totalResponseTime.ToString() + ".";
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = sMsg;
	Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::showFileUploadReplies);
	Dispatcher->Invoke(act, args);  // must call showFileUploadReplies on main UI thread
};

void ClientWPF::fileDownloadReplyDispatch(Message& ms){
	std::string servernumber, serverrelativepath, localpath;
	servernumber = ms.getAttributes()["servernumber"];
	serverrelativepath = ms.getAttributes()["serverrelativepath"];
	localpath = ms.getAttributes()["localpath"];

	// display the server's reply of file upload on client's UI
	std::string downloadresult;
	downloadresult = ms.getFiletoUpload() + " from " + servernumber + " under " + serverrelativepath + " has been downloaded to local path " + localpath;
	String^ sMsg = toSystemString(downloadresult);
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = sMsg;
	Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::showFileDownloadResult);
	Dispatcher->Invoke(act, args);
}

void ClientWPF::textSearchReplyDispatch(Message& ms){
	std::string asxml, servernumber, numberoffiles;
	asxml = ms.getAttributes()["asxml"];
	servernumber = ms.getAttributes()["servernumber"];
	numberoffiles = ms.getAttributes()["numberoffiles"];
	// display the server's reply of text search on client's UI
	std::string searchresult = ms.getBody();
	String^ sMsg = toSystemString(searchresult);
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = sMsg;
	if (asxml == "False"){
		Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::showTextSearchResult);
		Dispatcher->Invoke(act, args);
	}
	else{
		Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::showTextSearchResultXML);
		Dispatcher->Invoke(act, args);
	}
	std::string searchsummary = servernumber + " has returned " + numberoffiles + " files";
	String^ sMsg2 = toSystemString(searchsummary);
	array<String^>^ args2 = gcnew array<String^>(1);
	args2[0] = sMsg2;
	Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::showTextSearchSummary);
	Dispatcher->Invoke(act, args2);
}

void ClientWPF::quitReplyDispatch(Message& ms){
	std::string msg = "Client receiver is going to stop";
	String^ sMsg = toSystemString(msg);
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = sMsg;
	Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::addStatusText);
	isRecvThreadEnded = true;
}

void ClientWPF::fileSearchReplyDispatch(Message& ms){
	std::string asxml, servernumber, numberoffiles, numberofdirectories;
	asxml = ms.getAttributes()["asxml"];
	servernumber = ms.getAttributes()["servernumber"];
	numberoffiles = ms.getAttributes()["numberoffiles"];
	numberofdirectories = ms.getAttributes()["numberofdirectories"];

	// display the server's reply of file search on client's UI
	std::string searchresult = ms.getBody();
	String^ sMsg = toSystemString(searchresult);
	array<String^>^ args = gcnew array<String^>(1);
	args[0] = sMsg;
	if (asxml == "False"){
		Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::showFileSearchResult);
		Dispatcher->Invoke(act, args);
	}
	else{
		Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::showFileSearchResultXML);
		Dispatcher->Invoke(act, args);
	}
	std::string searchsummary = servernumber + " has returned " + numberoffiles + " files from " + numberofdirectories + " directories";
	String^ sMsg2 = toSystemString(searchsummary);
	array<String^>^ args2 = gcnew array<String^>(1);
	args2[0] = sMsg2;
	Action<String^>^ act = gcnew Action<String^>(this, &ClientWPF::showFileSearchSummary);
	Dispatcher->Invoke(act, args2);
}

void ClientWPF::clear(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  cleared message text");
	hStatus->Text = "Cleared message";
	hTextBlock1->Text = "";
}

void ClientWPF::setUpFileUploadView()
{
	Console::Write("\n  setting up FileUpload view");
	hFileUploadGrid->Margin = Thickness(10);
	hFileUploadTab->Content = hFileUploadGrid;

	setFileUploadOptions();
	setFileUploadResults();
	setFileUploadControls();
}

void ClientWPF::setFileUploadOptions(){

	//The first row enable the input of server port number
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(40);
	hFileUploadGrid->RowDefinitions->Add(hRow1Def);

	Border^ hBorder11 = gcnew Border();
	hBorder11->Width = 320;
	hBorder11->Height = 20;
	hBorder11->Margin = Thickness(10, 0, 0, 0);
	hBorder11->BorderThickness = Thickness(0);
	hUploadServerPortTag->Text = "Server Port (Please provide an integer port number):";
	hUploadServerPortTag->TextAlignment = TextAlignment::Left;
	hBorder11->Child = hUploadServerPortTag;
	Border^ hBorder12 = gcnew Border();
	hBorder12->Width = 120;
	hBorder12->Height = 30;
	hBorder12->BorderThickness = Thickness(1);
	hBorder12->Child = hUploadServerPortTextBox;

	fileUploadStackPanel1->Children->Add(hBorder11);
	fileUploadStackPanel1->Children->Add(hBorder12);
	fileUploadStackPanel1->Orientation = Orientation::Horizontal;
	fileUploadStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

	hFileUploadGrid->SetRow(fileUploadStackPanel1, 0);
	hFileUploadGrid->Children->Add(fileUploadStackPanel1);
}

void ClientWPF::setFileUploadResults(){
	//The second row display a list box of local files under selected path
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(280);
	hFileUploadGrid->RowDefinitions->Add(hRow2Def);
	Border^ hBorder2 = gcnew Border();
	hBorder2->Height = 280;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hFileUploadListBox;

	hFileUploadGrid->SetRow(hBorder2, 1);
	hFileUploadGrid->Children->Add(hBorder2);

	//The third row display FileUpload replies from the servers
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(100);
	hFileUploadGrid->RowDefinitions->Add(hRow3Def);

	hFileUploadStatusBar->Items->Add(hFileUploadStatusBarItem);
	hFileUploadStatus->Text = "Upload replies will appear here: \n";
	hFileUploadStatus->TextWrapping = TextWrapping::Wrap;
	hUploadStatusScrollViewer->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hUploadStatusScrollViewer->Content = hFileUploadStatus;
	hUploadStatusScrollViewer->Height = 100;
	hUploadStatusScrollViewer->Width = 745;
	hUploadStatusScrollViewer->Padding = Thickness(10, 0, 0, 0);
	hFileUploadStatusBarItem->Content = hUploadStatusScrollViewer;

	hFileUploadGrid->SetRow(hFileUploadStatusBar, 2);
	hFileUploadGrid->Children->Add(hFileUploadStatusBar);
}

void ClientWPF::setFileUploadControls(){
	//The fourth row displays FolerBrowser Button and FileUpload Button
	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow4Def->Height = GridLength(45);
	hFileUploadGrid->RowDefinitions->Add(hRow4Def);

	hFileUploadClearButton->Content = "Clear";
	hFileUploadClearButton->Height = 30;
	hFileUploadClearButton->Width = 120;
	hFileUploadClearButton->Margin = Thickness(0, 15, 10, 0);
	hFileUploadClearButton->BorderThickness = Thickness(1);
	hFileUploadClearButton->BorderBrush = Brushes::Black;
	fileUploadStackPanel2->Children->Add(hFileUploadClearButton);

	hFolderBrowseButton->Content = "Select Directory";
	hFolderBrowseButton->Height = 30;
	hFolderBrowseButton->Width = 120;
	hFolderBrowseButton->Margin = Thickness(0, 15, 10, 0);
	hFolderBrowseButton->BorderThickness = Thickness(2);
	hFolderBrowseButton->BorderBrush = Brushes::Black;
	fileUploadStackPanel2->Children->Add(hFolderBrowseButton);

	hFileUploadButton->Content = "Upload";
	hFileUploadButton->Height = 30;
	hFileUploadButton->Width = 120;
	hFileUploadButton->Margin = Thickness(0, 15, 20, 0);
	hFileUploadButton->BorderThickness = Thickness(1);
	hFileUploadButton->BorderBrush = Brushes::Black;
	hFileUploadButton->Visibility = System::Windows::Visibility::Hidden;
	fileUploadStackPanel2->Children->Add(hFileUploadButton);

	fileUploadStackPanel2->Orientation = Orientation::Horizontal;
	fileUploadStackPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Right;
	hFileUploadGrid->SetRow(fileUploadStackPanel2, 3);
	hFileUploadGrid->Children->Add(fileUploadStackPanel2);

	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();

	hFileBrowseDialog->InitialDirectory = System::IO::Directory::GetCurrentDirectory();
}
void ClientWPF::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for folder";
	hFileUploadListBox->Items->Clear();
	System::Windows::Forms::DialogResult result;

	result = hFolderBrowserDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		//std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		for (int i = 0; i < files->Length; ++i)
			hFileUploadListBox->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			hFileUploadListBox->Items->Add(L"<> " + dirs[i]);
	}
}
void ClientWPF::setUpTextSearchView()
{
	Console::Write("\n  setting up TextSearch view");
	hTextSearchGrid->Margin = Thickness(10);
	hTextSearchTab->Content = hTextSearchGrid;

	setConnection1Options();
    setConnection2Options();
    setTextSearchOptions1();
	setTextSearchOptions2();
	setTextSearchControls();
	setTextSearchResults();
}

void ClientWPF::setConnection1Options(){
	//The first row enable the input of server 1 port and number of thread partipating in text search
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hRow1Def->Height = GridLength(33);
	hTextSearchGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder11 = gcnew Border();
	hBorder11->Width = 90;
	hBorder11->Height = 20;
	hBorder11->Margin = Thickness(40, 0, 0, 0);
	hBorder11->BorderThickness = Thickness(0);
	hTextSearchServer1PortTag->Text = "Server1 Port:";
	hTextSearchServer1PortTag->TextAlignment = TextAlignment::Left;
	hBorder11->Child = hTextSearchServer1PortTag;
	Border^ hBorder12 = gcnew Border();
	hBorder12->Width = 120;
	hBorder12->Height = 30;
	hBorder12->Margin = Thickness(0, 0, 20, 0);
	hBorder12->BorderThickness = Thickness(1);
	hBorder12->Child = hTextSearchServer1PortTextBox;
	Border^ hBorder13 = gcnew Border();
	hBorder13->Width = 210;
	hBorder13->Height = 20;
	hBorder13->Margin = Thickness(10, 0, 0, 0);
	hBorder13->BorderThickness = Thickness(0);
	hTextSearchServer1ThreadsTag->Text = "Number of Participation Threads:";
	hTextSearchServer1ThreadsTag->TextAlignment = TextAlignment::Left;
	hBorder13->Child = hTextSearchServer1ThreadsTag;
	Border^ hBorder14 = gcnew Border();
	hBorder14->Width = 120;
	hBorder14->Height = 30;
	hBorder14->BorderThickness = Thickness(1);
	hTextSearchServer1ThreadsComboItem1->Content = 1;
	hTextSearchServer1ThreadsComboItem2->Content = 2;
	hTextSearchServer1ThreadsComboItem3->Content = 3;
	hTextSearchServer1ThreadsComboItem4->Content = 4;
	hTextSearchServer1ThreadsComboBox->SelectedValuePath;
	hTextSearchServer1ThreadsComboBox->Items->Add(hTextSearchServer1ThreadsComboItem1);
	hTextSearchServer1ThreadsComboBox->Items->Add(hTextSearchServer1ThreadsComboItem2);
	hTextSearchServer1ThreadsComboBox->Items->Add(hTextSearchServer1ThreadsComboItem3);
	hTextSearchServer1ThreadsComboBox->Items->Add(hTextSearchServer1ThreadsComboItem4);
	hBorder14->Child = hTextSearchServer1ThreadsComboBox;
	textSearchStackPanel1->Children->Add(hBorder11);
	textSearchStackPanel1->Children->Add(hBorder12);
	textSearchStackPanel1->Children->Add(hBorder13);
	textSearchStackPanel1->Children->Add(hBorder14);
	textSearchStackPanel1->Orientation = Orientation::Horizontal;
	textSearchStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;
	hTextSearchGrid->SetRow(textSearchStackPanel1, 0);
	hTextSearchGrid->Children->Add(textSearchStackPanel1);
};
void ClientWPF::setConnection2Options(){
	//The second row enable the input of server 2 port and number of thread partipating in text search
	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(33);
	hTextSearchGrid->RowDefinitions->Add(hRow2Def);
	Border^ hBorder21 = gcnew Border();
	hBorder21->Width = 90;
	hBorder21->Height = 20;
	hBorder21->Margin = Thickness(40, 0, 0, 0);
	hBorder21->BorderThickness = Thickness(0);
	hTextSearchServer2PortTag->Text = "Server2 Port:";
	hTextSearchServer2PortTag->TextAlignment = TextAlignment::Left;
	hBorder21->Child = hTextSearchServer2PortTag;
	Border^ hBorder22 = gcnew Border();
	hBorder22->Width = 120;
	hBorder22->Height = 30;
	hBorder22->Margin = Thickness(0, 0, 20, 0);
	hBorder22->BorderThickness = Thickness(1);
	hBorder22->Child = hTextSearchServer2PortTextBox;
	Border^ hBorder23 = gcnew Border();
	hBorder23->Width = 210;
	hBorder23->Height = 20;
	hBorder23->Margin = Thickness(10, 0, 0, 0);
	hBorder23->BorderThickness = Thickness(0);
	hTextSearchServer2ThreadsTag->Text = "Number of Participation Threads:";
	hTextSearchServer2ThreadsTag->TextAlignment = TextAlignment::Left;
	hBorder23->Child = hTextSearchServer2ThreadsTag;
	Border^ hBorder24 = gcnew Border();
	hBorder24->Width = 120;
	hBorder24->Height = 30;
	hBorder24->BorderThickness = Thickness(1);
	hTextSearchServer2ThreadsComboItem1->Content = 1;
	hTextSearchServer2ThreadsComboItem2->Content = 2;
	hTextSearchServer2ThreadsComboItem3->Content = 3;
	hTextSearchServer2ThreadsComboItem4->Content = 4;
	hTextSearchServer2ThreadsComboBox->SelectedValuePath = "Content";
	hTextSearchServer2ThreadsComboBox->Items->Add(hTextSearchServer2ThreadsComboItem1);
	hTextSearchServer2ThreadsComboBox->Items->Add(hTextSearchServer2ThreadsComboItem2);
	hTextSearchServer2ThreadsComboBox->Items->Add(hTextSearchServer2ThreadsComboItem3);
	hTextSearchServer2ThreadsComboBox->Items->Add(hTextSearchServer2ThreadsComboItem4);
	hBorder24->Child = hTextSearchServer2ThreadsComboBox;
	textSearchStackPanel2->Children->Add(hBorder21);
	textSearchStackPanel2->Children->Add(hBorder22);
	textSearchStackPanel2->Children->Add(hBorder23);
	textSearchStackPanel2->Children->Add(hBorder24);
	textSearchStackPanel2->Orientation = Orientation::Horizontal;
	textSearchStackPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;
	hTextSearchGrid->SetRow(textSearchStackPanel2, 1);
	hTextSearchGrid->Children->Add(textSearchStackPanel2);
};

void ClientWPF::setTextSearchOptions1(){
	//The third row enable the input of file patterns to search
	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(33);
	hTextSearchGrid->RowDefinitions->Add(hRow3Def);

	Border^ hBorder31 = gcnew Border();
	hBorder31->Width = 90;
	hBorder31->Height = 20;
	hBorder31->Margin = Thickness(40, 0, 0, 0);
	hBorder31->BorderThickness = Thickness(0);
	hTextSearchFilePatternsTag->Text = "File Patterns:";
	hTextSearchFilePatternsTag->TextAlignment = TextAlignment::Left;
	hBorder31->Child = hTextSearchFilePatternsTag;
	Border^ hBorder32 = gcnew Border();
	hBorder32->Width = 480;
	hBorder32->Height = 30;
	hBorder32->Margin = Thickness(0, 0, 20, 0);
	hBorder32->BorderThickness = Thickness(1);
	hBorder32->Child = hTextSearchFilePatternsTextBox;

	textSearchStackPanel3->Children->Add(hBorder31);
	textSearchStackPanel3->Children->Add(hBorder32);
	textSearchStackPanel3->Orientation = Orientation::Horizontal;
	textSearchStackPanel3->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

	hTextSearchGrid->SetRow(textSearchStackPanel3, 2);
	hTextSearchGrid->Children->Add(textSearchStackPanel3);
};

void ClientWPF::setTextSearchOptions2(){
	//The fourth row enable the input of search text
	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow4Def->Height = GridLength(33);
	hTextSearchGrid->RowDefinitions->Add(hRow4Def);

	Border^ hBorder41 = gcnew Border();
	hBorder41->Width = 90;
	hBorder41->Height = 20;
	hBorder41->Margin = Thickness(40, 0, 0, 0);
	hBorder41->BorderThickness = Thickness(0);
	hTextSearchTextTag->Text = "Search Text:";
	hTextSearchTextTag->TextAlignment = TextAlignment::Left;
	hBorder41->Child = hTextSearchTextTag;
	Border^ hBorder42 = gcnew Border();
	hBorder42->Width = 480;
	hBorder42->Height = 30;
	hBorder42->Margin = Thickness(0, 0, 20, 0);
	hBorder42->BorderThickness = Thickness(1);
	hBorder42->Child = hTextSearchTextTextBox;

	textSearchStackPanel4->Children->Add(hBorder41);
	textSearchStackPanel4->Children->Add(hBorder42);
	textSearchStackPanel4->Orientation = Orientation::Horizontal;
	textSearchStackPanel4->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

	hTextSearchGrid->SetRow(textSearchStackPanel4, 3);
	hTextSearchGrid->Children->Add(textSearchStackPanel4);
}

void ClientWPF::setTextSearchResults(){
	//The sixth row display a list box of matched files from servers
	RowDefinition^ hRow6Def = gcnew RowDefinition();
	//hRow6Def->Height = GridLength(240);
	hTextSearchGrid->RowDefinitions->Add(hRow6Def);

	hTextSearchListBox->BorderThickness = Thickness(1);
	Border^ hXMLTextBlockBorder = gcnew Border();
	hXMLTextBlockBorder->BorderThickness = Thickness(1);
	hXMLTextBlockBorder->BorderBrush = Brushes::Black;
	hTextSearchXMLTextBlock->TextWrapping = TextWrapping::Wrap;
	hXMLTextBlockBorder->Child = hTextSearchXMLTextBlock;
	hTextSearchXMLScrollViewer->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hTextSearchXMLScrollViewer->Content = hXMLTextBlockBorder;

	hTextSearchGrid->SetRow(hTextSearchListBox, 5);
	hTextSearchGrid->Children->Add(hTextSearchListBox);

	//The seventh row display text search performance information
	RowDefinition^ hRow7Def = gcnew RowDefinition();
	hRow7Def->Height = GridLength(80);
	hTextSearchGrid->RowDefinitions->Add(hRow7Def);

	hTextSearchStatusBar->Items->Add(hTextSearchStatusBarItem);
	hTextSearchStatus->Text = "Text search performance inforamtion will appear here: \n";
	hTextSearchStatus->TextWrapping = TextWrapping::Wrap;
	hTextSearchStatusScrollViewer->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hTextSearchStatusScrollViewer->Content = hTextSearchStatus;
	hTextSearchStatusScrollViewer->Height = 80;
	hTextSearchStatusScrollViewer->Width = 615;
	hTextSearchStatusScrollViewer->Padding = Thickness(10, 0, 10, 0);
	hTextSearchStatusBarItem->Content = hTextSearchStatusScrollViewer;
	hTextSearchClearButton->Content = "Clear";
	hTextSearchClearButton->Height = 30;
	hTextSearchClearButton->Width = 102;
	hTextSearchClearButton->Margin = Thickness(5, 0, 0, 20);
	hTextSearchClearButton->BorderThickness = Thickness(1);

	textSearchStackPanel7->Children->Add(hTextSearchStatusBar);
	textSearchStackPanel7->Children->Add(hTextSearchClearButton);
	textSearchStackPanel7->Orientation = Orientation::Horizontal;
	textSearchStackPanel7->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

	hTextSearchGrid->SetRow(textSearchStackPanel7, 6);
	hTextSearchGrid->Children->Add(textSearchStackPanel7);
};
void ClientWPF::setTextSearchControls(){
	//The fifth row displays text search button and XML string format checkbox
	RowDefinition^ hRow5Def = gcnew RowDefinition();
	hRow5Def->Height = GridLength(36);
	hTextSearchGrid->RowDefinitions->Add(hRow5Def);

	hTextSearchButton->Content = "Search";
	hTextSearchButton->Height = 30;
	hTextSearchButton->Width = 120;
	hTextSearchButton->Margin = Thickness(130, 3, 45, 3);
	hTextSearchButton->BorderThickness = Thickness(1);
	hTextSearchXMLCheckBox->Content = "Display as XML string";
	hTextSearchXMLCheckBox->Height = 30;
	hTextSearchXMLCheckBox->Margin = Thickness(0, 8, 0, 0);
	textSearchStackPanel5->Children->Add(hTextSearchButton);
	textSearchStackPanel5->Children->Add(hTextSearchXMLCheckBox);
	textSearchStackPanel5->Orientation = Orientation::Horizontal;
	textSearchStackPanel5->HorizontalAlignment = System::Windows::HorizontalAlignment::Left;

	hTextSearchGrid->SetRow(textSearchStackPanel5, 4);
	hTextSearchGrid->Children->Add(textSearchStackPanel5);
};

void ClientWPF::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
	Console::Write("\n  Window loaded");
}
void ClientWPF::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
	Console::Write("\n  Window closing");
}

[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
	String^ arg1 = "9080";
	String^ arg2 = "Client1Folder/";
	if (args->Length > 0) arg1 = args[0];
	if (args->Length > 1) arg2 = args[1];
	Console::WriteLine(L"\n Starting ClientWPF (port:" + arg1 + ", loc: " + arg2 + ")");
	Application^ app = gcnew Application();
	app->Run(gcnew ClientWPF(arg1, arg2));
	Console::WriteLine(L"\n\n");
}