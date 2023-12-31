#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QPushButton"
#include "QMessageBox"
#include "QRandomGenerator"
#include <openssl/evp.h>
#include "QByteArray"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show_pin_check_window() {

    pin_window = new QWidget(); //Создание виджета как окна для ввода пин кода
    pin_window->setGeometry(683,350,270,120);

    QVBoxLayout *layout_pin = new QVBoxLayout;

    QLabel *pin_code_label = new QLabel("Пин код"); //Лейбл для пинкода
    input_pincode = new QLineEdit(); // Инпут для пинкода
    input_pincode->setEchoMode(QLineEdit::Password);

    QPushButton *check_pin_btn = new QPushButton("Вход"); //Кнопка для проверки
    connect(check_pin_btn, SIGNAL(clicked()), this, SLOT(check_pin_code())); //При нажатии на кнопку отправляемся в функцию проверки

    //Добавляем в окно лейбл кнопку и инпут
    layout_pin->addWidget(pin_code_label);
    layout_pin->addWidget(input_pincode);
    layout_pin->addWidget(check_pin_btn);
    pin_window->setLayout(layout_pin);

    pin_window->show();
}

int MainWindow::check_pin_code() {
    const QString orig_code = "1234";
    QString code = input_pincode->text();

    if (orig_code == code) {
        delete[] pin_window;
        this->show_game_window(); //запускаем основное окно
        this->show();
        return 0;

    }
    else {
        QMessageBox::critical(NULL,QObject::tr("Ошибка"),tr("Ошибка ввода пин-кода"));
        return 0;
    }
}


int MainWindow::show_game_window() {

    //Создание маперов для кнопок
    game_mapper = new QSignalMapper(this);
    QObject::connect(game_mapper,SIGNAL(mappedInt(int)),this,SLOT(on_click_game_btn(int))); //сигнал передачу id кнопки при нажатии на кнопку

    list_of_number[8]; //Массив хранит в себе значения рандомных чисел

    //Создание кнопок
    for (int i = 0; i < 9; i++) {
        QPushButton* game_btn = create_game_btn(i); //Создание кнопки
        game_btn_list.push_back(game_btn); // Добавление в общий лист

        quint32 number = QRandomGenerator::global()->bounded(-1000,1000); //Создание рандомного числа

        list_of_number[i] = (int)number;//Добавление в масив рандомных чисел

        //Шифрование
        unsigned char chipr[512];
        QByteArray number_bit;
        number_bit.setNum(number);
        int encrypt_number = encrypt((unsigned char *)number_bit.data(), number_bit.length(), key, iv, chipr);
        QByteArray chipr_bit =  QByteArray::fromRawData((const char*)chipr, encrypt_number).toBase64();
        qDebug() << "chipr_bit --- " + chipr_bit;
        list_of_encrypt_number[i] = chipr_bit;
    }

    //Добавление кнопок в графическое окно
    for (int i = 0; i < game_btn_list.length();) {
        QHBoxLayout *Hlayout = new QHBoxLayout;
        for (int j = 0; j < 3; j++, i++) {
            Hlayout->addWidget(game_btn_list[i]);//Добавляем в гориз лайаут кнопку из листа
        }
        ui->verticalLayout->addLayout(Hlayout);
    }

    return 0;
}

QPushButton* MainWindow::create_game_btn(int id) {

    QPushButton *current_game_btn = new QPushButton("Click");//Создание кнопки

    game_mapper->setMapping(current_game_btn, id); //Вместе с кнопкой передаем id
    connect(current_game_btn,SIGNAL(clicked()),game_mapper,SLOT(map())); //слот на передачу при нажатии

    return current_game_btn;
}

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

int MainWindow::decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                   unsigned char *iv, unsigned char *decryptext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int decryptext_len;

    if(!(ctx = EVP_CIPHER_CTX_new()))
        crypt_error();


    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        crypt_error();


    if(1 != EVP_DecryptUpdate(ctx, decryptext, &len, ciphertext, ciphertext_len))
        crypt_error();
    decryptext_len = len;


    if(1 != EVP_DecryptFinal_ex(ctx, decryptext + len, &len))
        crypt_error();
    decryptext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return decryptext_len;
}

int MainWindow::crypt_error(void) {
    return 1;
}
