#include "size_selector.h"

SizeSelector::SizeSelector(
    size_t initial_size,
    QWidget* parent
) :
    QWidget(parent) 
{
    size = initial_size;
    edit_state = false;

    init_ui();
    connect_signals();
    update_ui_visibility();
}

void SizeSelector::setSize(size_t size) {
    this->size = size;
    edit_state = false;
    lbl_size->setText(QString::number(size));
    update_ui_visibility();
}

void SizeSelector::init_ui() {
    layout = new QHBoxLayout(this);

    lbl_size = new QLabel(QString::number(size), this);
    le_size = new QLineEdit(this);

    exp = new QRegularExpression("^(0|[1-9]\\d*)$");
    validator = new QRegularExpressionValidator(*exp, le_size);
    
    le_size->setValidator(validator);

    layout->addWidget(lbl_size);
    layout->addWidget(le_size);

    ico_edit = QIcon::fromTheme("edit-entry");
    ico_confirm = QIcon::fromTheme("dialog-ok");
    btn_edit = new QPushButton(this);
    #if defined (_WIN32)
    btn_edit->setText("Изменить");
    #else
    btn_edit->setIcon(ico_edit);
    #endif
    layout->addStretch();
    layout->addWidget(btn_edit);
}

void SizeSelector::update_ui_visibility() {
    le_size->setVisible(edit_state);

    lbl_size->setVisible(!edit_state);

    #if defined (_WIN32)
    btn_edit->setText(edit_state ? "Подтвердить" : "Изменить");
    #else
    btn_edit->setIcon(edit_state ? ico_confirm : ico_edit);
    #endif
}

void SizeSelector::connect_signals() {
    connect(btn_edit, &QPushButton::pressed, this, &SizeSelector::on_btn_edit);
}

void SizeSelector::on_btn_edit() {
    if (!edit_state) {
        edit_state = true;
        le_size->setText(QString::number(size));
        update_ui_visibility();
        return;
    }

    edit_state = false;
    update_ui_visibility();

    QString str_size = le_size->text();
    bool ok = false;
    size_t parsed_size = str_size.toULongLong(&ok);
    if (!ok)
        return;

    size = parsed_size;
    lbl_size->setText(QString::number(size));

    emit sig_size_changed(size);
}