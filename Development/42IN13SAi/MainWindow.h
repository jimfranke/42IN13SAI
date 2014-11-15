#pragma once

#include <QMainWindow>
#include <QObject>
#include "Highlighter.h"
class CodeEditor;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
		MainWindow(QWidget *parent = 0);
		virtual ~MainWindow();
		QString GetText();
		void SetText(QString text);
		QAction* GetRunAction();
		QAction* GetClearAction();
		QAction* GetLoadFileAction();
		QString OpenFileDialog();

	private:
		CodeEditor* codeEditor;
		Highlighter* highlighter;

		QMenu *fileMenu;
		QAction *runAction;
		QAction *clearAction;
		QAction *openFile;

		void ShowMenuBar();
};