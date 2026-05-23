#include <QApplication>
#include "MainWindow.h"
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.setStyleSheet("QMainWindow { background-color: #282c34; }"
                      "QMenuBar { background-color: #21252b; color: #abb2bf; }"
                      "QMenuBar::item:selected { background-color: #3e4451; }"
                      "QMenu { background-color: #21252b; color: #abb2bf; border: 1px solid #181a1f; }"
                      "QMenu::item:selected { background-color: #3e4451; }"
                      "QTabWidget::pane { border: 0px; }"
                      "QTabBar::tab { background: #21252b; color: #636d83; padding: 8px 16px; border-right: 1px solid #181a1f; }"
                      "QTabBar::tab:selected { background: #282c34; color: #abb2bf; }"
                      "QStatusBar { background-color: #21252b; color: #abb2bf; border-top: 1px solid #181a1f; }"
                      "QMessageBox { background-color: #282c34; color: #abb2bf; }"
                      "QMessageBox QLabel { color: #abb2bf; }"
                      "QMessageBox QPushButton { background-color: #3e4451; color: #abb2bf; padding: 5px; border: none; }"
                      "QMessageBox QPushButton:hover { background-color: #4b5263; }");
    MainWindow window;
    window.show();
    return app.exec();
}
