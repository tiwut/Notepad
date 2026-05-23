#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
class CodeEditor;
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
protected:
    void closeEvent(QCloseEvent *event) override;
private slots:
    void newFile();
    void openFile();
    bool saveFile();
    bool saveFileAs();
    void saveAllFiles();
    void closeTab(int index);
    void documentModified();
    void updateStatusBar();
    void toggleWordWrap(bool checked);
    void toggleFullScreen(bool checked);
    void zoomIn();
    void zoomOut();
    void cutText();
    void copyText();
    void pasteText();
    void undoText();
    void redoText();
    void selectAllText();
private:
    void createActions();
    void createStatusBar();
    CodeEditor* createNewEditor();
    CodeEditor* currentEditor();
    bool maybeSave(int index);
    void setCurrentFile(int index, const QString &fileName);
    QTabWidget *tabWidget;
    QLabel *statusLabel;
};
#endif
