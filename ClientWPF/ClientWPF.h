#ifndef CLIENTWPF_H
#define CLIENTWPF_H
/////////////////////////////////////////////////////////////////////
// ClientWPF.h - C++\CLI implementation of WPF Application         //
//          -  CSE 687 Project #4, 2015                            //
// Ver 1.0                                                         //
// Language:    Visual C++, Visual Studio 2013                     //
// platform:    Windows 7                                          //
// Application: CSE687 2015 - Project 4 - Remote Code Management   //
// Author:      Wenting Wang, wwang34@syr.edu                      //
// Source:      Jim Fawcett, jfawcett@twcny.rr.com                 //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package is a WPF Application.  It uses C++\CLI to construction a
* window application that allow users to interact with the program via
* UI interface. It provides one class, WPFCppCliDemo, derived from 
* System::Windows::Window that is compiled with the /clr option to run 
* in the Common Language Runtime, and another class Client written in 
* native C++ and compiled as a DLL with no Common Language Runtime 
* support.
*
* The window class hosts, in its window, a tab control with three views for
* file search, file upload and text search respectively. File download are
* also provide in file search view, after a file list is returned, download
* option is provided. It loads the DLL holding Client.  Client hosts a 
* sender and a receiver. The sender and receiver both have their blocking
* queues and sending and receiving threads respectively. A user make 
* requests via UI, and the request is sent to the server via client's 
* sender. Server has the same structure with the client, except it doesn't
* have a UI interface. It receives and processes client's requests and 
* send back to client replies, these replies are displayed on the UI as 
* well.
*
* Required Files:
* ---------------
*   - Message.h, Message.cpp,
*     Client.lib
*
* Build Process:
* --------------
*   devenv ClientWPF.sln /rebuild Debug
*
*/
using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;

#include "../Client/IClient.h"
#include <iostream>

namespace CppCliWindows
{
	ref class ClientWPF : Window
	{
		// Client references
		IClient* pClient;
		ISender* pSender;
		IReceiver* pReceiver;

		// Controls for Window

		DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
		Grid^ hGrid = gcnew Grid();
		TabControl^ hTabControl = gcnew TabControl();
		TabItem^ hFileSearchTab = gcnew TabItem();
		TabItem^ hSendMessageTab = gcnew TabItem();
		TabItem^ hFileUploadTab = gcnew TabItem();
		TabItem^ hTextSearchTab = gcnew TabItem();
		StatusBar^ hStatusBar = gcnew StatusBar();
		StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hStatus = gcnew TextBlock();

		// Controls for FileSearch View
		Grid^ hFileSearchGrid = gcnew Grid();
		TextBlock^ hServerPortTag = gcnew TextBlock();
		TextBox^ hFileSearchServerPortTextBox = gcnew TextBox();
		StackPanel^ fileSearchParamStackPanel1 = gcnew StackPanel();

		TextBlock^ hServerPathTag = gcnew TextBlock();
		TextBox^ hServerPathTextBox = gcnew TextBox();
		StackPanel^ fileSearchParamStackPanel2 = gcnew StackPanel();

		TextBlock^ hFilePatternsTag = gcnew TextBlock();
		TextBox^ hFilePatternsTextBox = gcnew TextBox();
		StackPanel^ fileSearchParamStackPanel3 = gcnew StackPanel();

		Button^ hExploreButton = gcnew Button();
		CheckBox^ hFileSearchXMLCheckBox = gcnew CheckBox();
		Button^ hFileSearchButton = gcnew Button();
		StackPanel^ fileSearchButtonStackPanel = gcnew StackPanel();

		RowDefinition^ hRow5Def = gcnew RowDefinition();
		ListBox^ hFileSearchListBox = gcnew ListBox();
		ScrollViewer^ hFileSearchXMLScrollViewer = gcnew ScrollViewer();
		TextBlock^ hFileSearchXMLTextBlock = gcnew TextBlock();

		StackPanel^ fileDownloadButtonStackPanel = gcnew StackPanel();
		ScrollViewer^ hFileSearchStatusScrollViewer = gcnew ScrollViewer();
		StatusBar^ hFileSearchStatusBar = gcnew StatusBar();
		StatusBarItem^ hFileSearchStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hFileSearchStatus = gcnew TextBlock();
		Button^ hFileSearchClearButton = gcnew Button();
		Button^ hDownloadButton = gcnew Button();

		// Controls for SendMessage View

		Grid^ hSendMessageGrid = gcnew Grid();
		Button^ hSendButton = gcnew Button();
		Button^ hClearButton = gcnew Button();
		TextBlock^ hTextBlock1 = gcnew TextBlock();
		ScrollViewer^ hScrollViewer1 = gcnew ScrollViewer();
		StackPanel^ hStackPanel1 = gcnew StackPanel();

		String^ msgText
			= "Command:ShowMessage\n"   // command
			+ "Sendr:localhost@8080\n"  // send address
			+ "Recvr:localhost@8090\n"  // receive address
			+ "Content-length:44\n"     // body length attribute
			+ "\n"                      // end header
			+ "Hello World\nCSE687 - Object Oriented Design";  // message body

		
		// Controls for FileUploadView 

		Grid^ hFileUploadGrid = gcnew Grid();
		Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
		Forms::OpenFileDialog^ hFileBrowseDialog = gcnew Forms::OpenFileDialog();
		StackPanel^ fileUploadStackPanel1 = gcnew StackPanel();
		TextBlock^ hUploadServerPortTag = gcnew TextBlock();
		TextBox^ hUploadServerPortTextBox = gcnew TextBox();
		ListBox^ hFileUploadListBox = gcnew ListBox();
		StatusBar^ hFileUploadStatusBar = gcnew StatusBar();
		StatusBarItem^ hFileUploadStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hFileUploadStatus = gcnew TextBlock();
		ScrollViewer^ hUploadStatusScrollViewer = gcnew ScrollViewer();
		StackPanel^ fileUploadStackPanel2 = gcnew StackPanel();
		Button^ hFileUploadClearButton = gcnew Button();
		Button^ hFolderBrowseButton = gcnew Button();
		Button^ hFileBrowseButton = gcnew Button();
		Button^ hFileUploadButton = gcnew Button();
		Grid^ hGrid2 = gcnew Grid();


		// Controls for TextSearchView 

		Grid^ hTextSearchGrid = gcnew Grid();
		StackPanel^ textSearchStackPanel1 = gcnew StackPanel();
		TextBlock^ hTextSearchServer1PortTag = gcnew TextBlock();
		TextBox^ hTextSearchServer1PortTextBox = gcnew TextBox();
		TextBlock^ hTextSearchServer1ThreadsTag = gcnew TextBlock();
		ComboBox^ hTextSearchServer1ThreadsComboBox = gcnew ComboBox();
		ComboBoxItem^ hTextSearchServer1ThreadsComboItem1 = gcnew ComboBoxItem();
		ComboBoxItem^ hTextSearchServer1ThreadsComboItem2 = gcnew ComboBoxItem();
		ComboBoxItem^ hTextSearchServer1ThreadsComboItem3 = gcnew ComboBoxItem();
		ComboBoxItem^ hTextSearchServer1ThreadsComboItem4 = gcnew ComboBoxItem();

		StackPanel^ textSearchStackPanel2 = gcnew StackPanel();
		TextBlock^ hTextSearchServer2PortTag = gcnew TextBlock();
		TextBox^ hTextSearchServer2PortTextBox = gcnew TextBox();
		TextBlock^ hTextSearchServer2ThreadsTag = gcnew TextBlock();
		ComboBox^ hTextSearchServer2ThreadsComboBox = gcnew ComboBox();
		ComboBoxItem^ hTextSearchServer2ThreadsComboItem1 = gcnew ComboBoxItem();
		ComboBoxItem^ hTextSearchServer2ThreadsComboItem2 = gcnew ComboBoxItem();
		ComboBoxItem^ hTextSearchServer2ThreadsComboItem3 = gcnew ComboBoxItem();
		ComboBoxItem^ hTextSearchServer2ThreadsComboItem4 = gcnew ComboBoxItem();

		StackPanel^ textSearchStackPanel3 = gcnew StackPanel();
		TextBlock^ hTextSearchFilePatternsTag = gcnew TextBlock();
		TextBox^ hTextSearchFilePatternsTextBox = gcnew TextBox();

		StackPanel^ textSearchStackPanel4 = gcnew StackPanel();
		TextBlock^ hTextSearchTextTag = gcnew TextBlock();
		TextBox^ hTextSearchTextTextBox = gcnew TextBox();

		StackPanel^ textSearchStackPanel5 = gcnew StackPanel();
		Button^ hTextSearchButton = gcnew Button();
		CheckBox^ hTextSearchXMLCheckBox = gcnew CheckBox();
		
		ListBox^ hTextSearchListBox = gcnew ListBox();
		ScrollViewer^ hTextSearchXMLScrollViewer = gcnew ScrollViewer();
		TextBlock^ hTextSearchXMLTextBlock = gcnew TextBlock();

		StackPanel^ textSearchStackPanel7 = gcnew StackPanel();
		ScrollViewer^ hTextSearchStatusScrollViewer = gcnew ScrollViewer();	
		StatusBar^ hTextSearchStatusBar = gcnew StatusBar();
		StatusBarItem^ hTextSearchStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hTextSearchStatus = gcnew TextBlock();
		Button^ hTextSearchClearButton = gcnew Button();

		// receive thread

		Thread^ recvThread;

	public:
		ClientWPF(String^ arg1, String^ arg2g);
		~ClientWPF();

		void setUpStatusBar();
		void setUpTabControl();
		void setupFileSearchView();
		void setUpSendMessageView();
		void setUpFileUploadView();
		void setUpTextSearchView();

		void sendFileSearchRequest(Object^ obj, RoutedEventArgs^ args);
		void changeDownloadBttnVisibility(Object^ obj, SelectionChangedEventArgs ^ args);
		void sendFileDownloadRequest(Object^ obj, RoutedEventArgs^ args);
		void clearFileSearchView(Object^ obj, RoutedEventArgs^ args);
		void showFileSearchResult(String^ sMsg);
		void showFileSearchResultXML(String^ sMsg);
		void showFileSearchSummary(String^ sMsg);
		void showFileDownloadResult(String^ msg);

		void showTextSearchResult(String^ sMsg);
		void showTextSearchResultXML(String^ sMsg);
		void showTextSearchSummary(String^ sMsg);

		void sendMessage(Object^ obj, RoutedEventArgs^ args);
		void sendFileUploadRequest(Object^ obj, RoutedEventArgs^ args);
		void clearFileUploadView(Object^ obj, RoutedEventArgs ^ args);
		void addText(String^ msg);
		void showFileUploadReplies(String^ msg);		
		void changeUploadBttnVisibility(Object^ obj, SelectionChangedEventArgs^ args);		
		void addStatusText(String^ msg);

		void sendTextSearchRequest(Object^ obj, RoutedEventArgs ^ args);
		void clearTextSearchView(Object^ obj, RoutedEventArgs ^ args);

		// Dispatches inside receive thread
		void fileDownloadReplyDispatch(Message& ms);
		void textSearchReplyDispatch(Message& ms);
		void fileSearchReplyDispatch(Message& ms);
		void fileUploadReplyDispatch(Message& ms);
		void quitReplyDispatch(Message& ms);

		void getMessage();
		void clear(Object^ sender, RoutedEventArgs^ args);
		void browseForFolder(Object^ sender, RoutedEventArgs^ args);
		void OnLoaded(Object^ sender, RoutedEventArgs^ args);
		void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);
	private:
		std::string toStdString(String^ pStr);
		String^ toSystemString(std::string& str);
		void setEventHandlersforFileSearch();
		void setEventHandlersforFileUpload();
		void setEventHandlersforTextSearch();
		void setTextBlockProperties();
		void setButtonsProperties();
		// set FileSearch View relevant parameters
		void setFileSearchOptions1();
		void setFileSearchOptions2();
		void setFileSearchControls1();
		void setFileSearchResults();
		void setFileSearchControls2();
		// set FileUpload View relevant parameters
		void setFileUploadOptions();
		void setFileUploadResults();
		void setFileUploadControls();
		// set TextSearch View relevant parameters
		void setConnection1Options();
		void setConnection2Options();
		void setTextSearchOptions1();
		void setTextSearchOptions2();
		void setTextSearchResults();
		void setTextSearchControls();
		String^ portstr_;
		bool isRecvThreadEnded;
	};
}


#endif
