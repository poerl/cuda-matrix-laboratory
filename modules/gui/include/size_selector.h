#pragma once

#include <cstddef>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <qwidget.h>
#include <QString>

class SizeSelector : public QWidget {
    Q_OBJECT
private:
    size_t size;

    QIcon ico_edit;
    QIcon ico_confirm;
    
    QLineEdit* le_size;
    QPushButton* btn_edit;
    QLabel* lbl_size;
    QHBoxLayout* layout;

    QRegularExpression* exp;
    QRegularExpressionValidator* validator;

    bool edit_state;
public:
    SizeSelector(
        size_t initial_size,
        QWidget* parent = nullptr
    );
public:
    void setSize(size_t size);
private:
    void init_ui();
    void update_ui_visibility();
    void connect_signals();
signals:
    void sig_size_changed(size_t size);
private slots:
    void on_btn_edit();
};