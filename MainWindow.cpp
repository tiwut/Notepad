#include "MainWindow.h"
#include "CodeEditor.h"
#include "Highlighter.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QCloseEvent>
#include <QStatusBar>
#include <QFileInfo>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    setCentralWidget(tabWidget);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(tabWidget, &QTabWidget::currentChanged, this, &MainWindow::updateStatusBar);
    createActions();
    createStatusBar();
    resize(1100, 750);
    setWindowTitle("CodePad");
    newFile();
}
void MainWindow::createActions() {
    QMenu *fileMenu = menuBar()->addMenu("File");
    QAction *newAct = new QAction("New", this);
    newAct->setShortcut(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    QAction *openAct = new QAction("Open...", this);
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    fileMenu->addAction(openAct);
    QAction *saveAct = new QAction("Save", this);
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveFile);
    fileMenu->addAction(saveAct);
    QAction *saveAsAct = new QAction("Save As...", this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveFileAs);
    fileMenu->addAction(saveAsAct);
    QAction *saveAllAct = new QAction("Save All", this);
    connect(saveAllAct, &QAction::triggered, this, &MainWindow::saveAllFiles);
    fileMenu->addAction(saveAllAct);
    fileMenu->addSeparator();
    QAction *exitAct = new QAction("Exit", this);
    exitAct->setShortcut(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAct);
    QMenu *editMenu = menuBar()->addMenu("Edit");
    QAction *undoAct = new QAction("Undo", this);
    undoAct->setShortcut(QKeySequence::Undo);
    connect(undoAct, &QAction::triggered, this, &MainWindow::undoText);
    editMenu->addAction(undoAct);
    QAction *redoAct = new QAction("Redo", this);
    redoAct->setShortcut(QKeySequence::Redo);
    connect(redoAct, &QAction::triggered, this, &MainWindow::redoText);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    QAction *cutAct = new QAction("Cut", this);
    cutAct->setShortcut(QKeySequence::Cut);
    connect(cutAct, &QAction::triggered, this, &MainWindow::cutText);
    editMenu->addAction(cutAct);
    QAction *copyAct = new QAction("Copy", this);
    copyAct->setShortcut(QKeySequence::Copy);
    connect(copyAct, &QAction::triggered, this, &MainWindow::copyText);
    editMenu->addAction(copyAct);
    QAction *pasteAct = new QAction("Paste", this);
    pasteAct->setShortcut(QKeySequence::Paste);
    connect(pasteAct, &QAction::triggered, this, &MainWindow::pasteText);
    editMenu->addAction(pasteAct);
    QAction *selectAllAct = new QAction("Select All", this);
    selectAllAct->setShortcut(QKeySequence::SelectAll);
    connect(selectAllAct, &QAction::triggered, this, &MainWindow::selectAllText);
    editMenu->addAction(selectAllAct);
    QMenu *viewMenu = menuBar()->addMenu("View");
    QAction *wordWrapAct = new QAction("Word Wrap", this);
    wordWrapAct->setCheckable(true);
    connect(wordWrapAct, &QAction::toggled, this, &MainWindow::toggleWordWrap);
    viewMenu->addAction(wordWrapAct);
    QAction *fullScreenAct = new QAction("Full Screen", this);
    fullScreenAct->setCheckable(true);
    fullScreenAct->setShortcut(QKeySequence::FullScreen);
    connect(fullScreenAct, &QAction::toggled, this, &MainWindow::toggleFullScreen);
    viewMenu->addAction(fullScreenAct);
    QAction *zoomInAct = new QAction("Zoom In", this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);
    viewMenu->addAction(zoomInAct);
    QAction *zoomOutAct = new QAction("Zoom Out", this);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);
    viewMenu->addAction(zoomOutAct);
}
void MainWindow::createStatusBar() {
    statusLabel = new QLabel("Ready");
    statusBar()->addWidget(statusLabel);
}
CodeEditor* MainWindow::createNewEditor() {
    CodeEditor *editor = new CodeEditor(this);
    Highlighter *highlighter = new Highlighter(editor->document());
    Q_UNUSED(highlighter)
    int index = tabWidget->addTab(editor, "Untitled");
    tabWidget->setCurrentIndex(index);
    connect(editor->document(), &QTextDocument::modificationChanged, this, &MainWindow::documentModified);
    connect(editor, &QPlainTextEdit::cursorPositionChanged, this, &MainWindow::updateStatusBar);
    return editor;
}
CodeEditor* MainWindow::currentEditor() {
    return qobject_cast<CodeEditor*>(tabWidget->currentWidget());
}
void MainWindow::newFile() {
    createNewEditor();
}
void MainWindow::openFile() {
    QStringList fileNames = QFileDialog::getOpenFileNames(this, "Open File(s)");
    for (const QString &fileName : fileNames) {
        CodeEditor *editor = createNewEditor();
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            editor->setPlainText(in.readAll());
            editor->setFilePath(fileName);
            QFileInfo fileInfo(fileName);
            int index = tabWidget->indexOf(editor);
            tabWidget->setTabText(index, fileInfo.fileName());
            editor->document()->setModified(false);
        }
    }
}
bool MainWindow::saveFile() {
    CodeEditor *editor = currentEditor();
    if (!editor) return false;
    if (editor->getFilePath().isEmpty()) {
        return saveFileAs();
    }
    QFile file(editor->getFilePath());
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        out << editor->toPlainText();
        editor->document()->setModified(false);
        QFileInfo fileInfo(editor->getFilePath());
        int index = tabWidget->indexOf(editor);
        tabWidget->setTabText(index, fileInfo.fileName());
        return true;
    }
    return false;
}
bool MainWindow::saveFileAs() {
    CodeEditor *editor = currentEditor();
    if (!editor) return false;
    QString fileName = QFileDialog::getSaveFileName(this, "Save As");
    if (fileName.isEmpty()) return false;
    editor->setFilePath(fileName);
    return saveFile();
}
void MainWindow::saveAllFiles() {
    int currentIndex = tabWidget->currentIndex();
    for (int i = 0; i < tabWidget->count(); ++i) {
        tabWidget->setCurrentIndex(i);
        saveFile();
    }
    tabWidget->setCurrentIndex(currentIndex);
}
void MainWindow::closeTab(int index) {
    if (maybeSave(index)) {
        QWidget *widget = tabWidget->widget(index);
        tabWidget->removeTab(index);
        delete widget;
        if (tabWidget->count() == 0) {
            newFile();
        }
    }
}
bool MainWindow::maybeSave(int index) {
    CodeEditor *editor = qobject_cast<CodeEditor*>(tabWidget->widget(index));
    if (!editor->document()->isModified()) {
        return true;
    }
    QMessageBox::StandardButton ret = QMessageBox::warning(this, "Unsaved Changes",
        "There are unsaved changes. Do you want to save?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save) {
        tabWidget->setCurrentIndex(index);
        return saveFile();
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
    return true;
}
void MainWindow::closeEvent(QCloseEvent *event) {
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (!maybeSave(i)) {
            event->ignore();
            return;
        }
    }
    event->accept();
}
void MainWindow::documentModified() {
    CodeEditor *editor = qobject_cast<CodeEditor*>(sender()->parent());
    if (editor) {
        int index = tabWidget->indexOf(editor);
        QString title = tabWidget->tabText(index);
        if (editor->document()->isModified() && !title.endsWith("*")) {
            tabWidget->setTabText(index, title + "*");
        } else if (!editor->document()->isModified() && title.endsWith("*")) {
            tabWidget->setTabText(index, title.left(title.length() - 1));
        }
    }
}
void MainWindow::updateStatusBar() {
    CodeEditor *editor = currentEditor();
    if (editor) {
        QTextCursor cursor = editor->textCursor();
        statusLabel->setText(QString("Line: %1  Col: %2").arg(cursor.blockNumber() + 1).arg(cursor.columnNumber() + 1));
    }
}
void MainWindow::toggleWordWrap(bool checked) {
    CodeEditor *editor = currentEditor();
    if (editor) {
        editor->setWordWrapMode(checked ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap);
    }
}
void MainWindow::toggleFullScreen(bool checked) {
    if (checked) showFullScreen();
    else showNormal();
}
void MainWindow::zoomIn() {
    CodeEditor *editor = currentEditor();
    if (editor) editor->zoomIn(1);
}
void MainWindow::zoomOut() {
    CodeEditor *editor = currentEditor();
    if (editor) editor->zoomOut(1);
}
void MainWindow::cutText() {
    CodeEditor *editor = currentEditor();
    if (editor) editor->cut();
}
void MainWindow::copyText() {
    CodeEditor *editor = currentEditor();
    if (editor) editor->copy();
}
void MainWindow::pasteText() {
    CodeEditor *editor = currentEditor();
    if (editor) editor->paste();
}
void MainWindow::undoText() {
    CodeEditor *editor = currentEditor();
    if (editor) editor->undo();
}
void MainWindow::redoText() {
    CodeEditor *editor = currentEditor();
    if (editor) editor->redo();
}
void MainWindow::selectAllText() {
    CodeEditor *editor = currentEditor();
    if (editor) editor->selectAll();
}
