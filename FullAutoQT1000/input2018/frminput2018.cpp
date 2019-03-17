#pragma execution_character_set("utf-8")

#include "frminput2018.h"
#include "ui_frminput2018.h"

#pragma execution_character_set("utf-8")

frmInput2018 *frmInput2018::self = NULL;
frmInput2018 *frmInput2018::Instance()
{
    if (!self) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new frmInput2018;
        }
    }

    return self;
}

frmInput2018::frmInput2018(QWidget *parent) : QWidget(parent), ui(new Ui::frmInput2018)
{
    ui->setupUi(this);
    this->initProperty();
    this->initNumber();
    this->initForm();
    QTimer::singleShot(1000, this, SLOT(initDb()));
}

frmInput2018::~frmInput2018()
{
    delete ui;
}

void frmInput2018::showEvent(QShowEvent *)
{
    this->update();
}

void frmInput2018::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton)) {
        this->move(e->globalPos() - mousePoint);
        this->update();
        e->accept();
    }
}

void frmInput2018::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void frmInput2018::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

//事件过滤器,用于识别鼠标单击汉字标签处获取对应汉字
bool frmInput2018::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            if (obj == ui->labCh1) {
                setChinese(0);
            } else if (obj == ui->labCh2) {
                setChinese(1);
            } else if (obj == ui->labCh3) {
                setChinese(2);
            } else if (obj == ui->labCh4) {
                setChinese(3);
            } else if (obj == ui->labCh5) {
                setChinese(4);
            } else if (currentType != "") {
                if (!isVisible()) {
                    showPanel();
                }
            }

            return false;
        } else {
            if (isVisible()) {
                hidePanel();
            }
        }
    } else if (event->type() == QEvent::KeyPress) {
        //如果输入法窗体不可见,则不需要处理
        if (!isVisible() && !mini) {
            return QWidget::eventFilter(obj, event);
        }

        QString labText = ui->labPY->text();
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        //Shift切换输入法模式,esc键关闭输入法面板,空格取第一个汉字,退格键删除
        //中文模式下回车键取拼音,中文模式下当没有拼音时可以输入空格
        if (keyEvent->key() == Qt::Key_Space) {
            if (labText != "") {
                ui->labPY->setText("");
                setChinese(0);
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            if (labText != "") {
                insertValue(labText);
                ui->labPY->setText("");
                selectChinese();
            } else {
                hidePanel();
            }

            return true;
        } else if (keyEvent->key() == Qt::Key_Control) {
            if (mini) {
                ui->btnType->click();
                return true;
            }
        } else if (keyEvent->key() == Qt::Key_Shift) {
            if (!mini) {
                ui->btnType->click();
                return true;
            }
        } else if (keyEvent->key() == Qt::Key_Escape) {
            ui->btnClose->click();
            return true;
        } else if (keyEvent->key() == Qt::Key_Backspace) {
            if (inputType == "chinese") {
                QString txt = labText;
                int len = txt.length();
                if (len > 0) {
                    ui->labPY->setText(txt.left(len - 1));
                    selectChinese();
                    return true;
                }
            }
        } else if (keyEvent->key() == Qt::Key_CapsLock) {
            ui->btnUpper->click();
            return true;
        } else if (keyEvent->text() == "+" || keyEvent->text() == "=") {
            if (labText != "") {
                ui->btnNext->click();
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->text() == "-" || keyEvent->text() == "_") {
            if (labText != "") {
                ui->btnPre->click();
                return true;
            } else {
                return false;
            }
        } else {
            if (currentType == "QWidget") {
                return false;
            }

            QString key;
            if (inputType == "chinese") {
                key = keyEvent->text();
            } else if (inputType == "english") {
                if (upper) {
                    key = keyEvent->text().toUpper();
                } else {
                    key = keyEvent->text().toLower();
                }
            }

            if (!key.isEmpty()) {
                QList<QPushButton *> btn = ui->widgetMain->findChildren<QPushButton *>();
                foreach (QPushButton *b, btn) {
                    QString text = b->text();
                    if (!text.isEmpty() && text == key) {
                        b->click();
                        return true;
                    }
                }
            }
        }

        return false;
    }

    return QWidget::eventFilter(obj, event);
}

void frmInput2018::initProperty()
{
    ui->btnDot->setProperty("btnOther", true);
    ui->btnSpace->setProperty("btnOther", true);
    ui->btnEnter->setProperty("btnOther", true);
    ui->btnDelete->setProperty("btnOther", true);
    ui->btnUpper->setProperty("btnOther", true);

    ui->btn0->setProperty("btnNum", true);
    ui->btn1->setProperty("btnNum", true);
    ui->btn2->setProperty("btnNum", true);
    ui->btn3->setProperty("btnNum", true);
    ui->btn4->setProperty("btnNum", true);
    ui->btn5->setProperty("btnNum", true);
    ui->btn6->setProperty("btnNum", true);
    ui->btn7->setProperty("btnNum", true);
    ui->btn8->setProperty("btnNum", true);
    ui->btn9->setProperty("btnNum", true);

    ui->btna->setProperty("btnLetter", true);
    ui->btnb->setProperty("btnLetter", true);
    ui->btnc->setProperty("btnLetter", true);
    ui->btnd->setProperty("btnLetter", true);
    ui->btne->setProperty("btnLetter", true);
    ui->btnf->setProperty("btnLetter", true);
    ui->btng->setProperty("btnLetter", true);
    ui->btnh->setProperty("btnLetter", true);
    ui->btni->setProperty("btnLetter", true);
    ui->btnj->setProperty("btnLetter", true);
    ui->btnk->setProperty("btnLetter", true);
    ui->btnl->setProperty("btnLetter", true);
    ui->btnm->setProperty("btnLetter", true);
    ui->btnn->setProperty("btnLetter", true);
    ui->btno->setProperty("btnLetter", true);
    ui->btnp->setProperty("btnLetter", true);
    ui->btnq->setProperty("btnLetter", true);
    ui->btnr->setProperty("btnLetter", true);
    ui->btns->setProperty("btnLetter", true);
    ui->btnt->setProperty("btnLetter", true);
    ui->btnu->setProperty("btnLetter", true);
    ui->btnv->setProperty("btnLetter", true);
    ui->btnw->setProperty("btnLetter", true);
    ui->btnx->setProperty("btnLetter", true);
    ui->btny->setProperty("btnLetter", true);
    ui->btnz->setProperty("btnLetter", true);

    labCh.append(ui->labCh1);
    labCh.append(ui->labCh2);
    labCh.append(ui->labCh3);
    labCh.append(ui->labCh4);
    labCh.append(ui->labCh5);

    //设置所有按钮输入法不可用
    QList<QPushButton *> btns = this->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setFocusPolicy(Qt::NoFocus);
        btn->setProperty("noinput", true);
        btn->setAutoRepeat(true);
        btn->setAutoRepeatDelay(500);
    }
}

void frmInput2018::initForm()
{
    QDesktopWidget desk;
    deskWidth = desk.availableGeometry().width();
    deskHeight = desk.availableGeometry().height();

    dbPath = qApp->applicationDirPath();
    mousePoint = QPoint();
    mousePressed = false;
    upper = false;

    currentWidget = 0;
    currentLineEdit = 0;
    currentTextEdit = 0;
    currentPlain = 0;
    currentBrowser = 0;
    currentType = "";

    //右下角输入法图标
    labStatus = new QLabel;
    labStatus->setAttribute(Qt::WA_TranslucentBackground);
    labStatus->setAlignment(Qt::AlignCenter);
    labStatus->setPixmap(QPixmap(":/image/status_en.png"));
    labStatus->setFixedSize(53, 27);
    labStatus->move(deskWidth - 58, deskHeight - 30);

#if (QT_VERSION > QT_VERSION_CHECK(5,0,0))
    setWindowFlags(Qt::Tool | Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    labStatus->setWindowFlags(Qt::Tool | Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
#else
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    labStatus->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
#endif

    //输入法面板的字体名称和按钮字体大小即汉字区域字体大小
    setFontInfo(this->font().family(), 11, 10);

    //输入法面板的固定大小
    setFrmSize(500, 200);
    //图标固定大小
    setIconSize(20, 20);
    //按钮之间的间隔
    setSpacing(6);
    //顶部汉字区域高度
    setTopHeight(40);

    //当前完整模式 false--完整模式有整个按键面板 true--迷你模式只有中英状态悬浮条和汉字备选栏
    setMini(false);

    //输入法面板的显示位置  control--显示在对应输入框的正下方 bottom--填充显示在底部  center--窗体居中显示
    setPosition("control");

    //输入法面板的样式  blue--淡蓝色   black--黑色  brown--灰黑色  gray--灰色  silvery--银色
    setStyle("black");

    //输入法模式 english--英文模式  chinese--中文模式  handwrite--手写模式  char--特殊字符模式
    setInputType("english");

    //设置默认普通模式
    number = true;
    setMode(false);

    //绑定按钮到事件
    QList<QPushButton *> btns = ui->widgetMain->findChildren<QPushButton *>();
    btns << ui->btnPre << ui->btnNext << ui->btn_;
    foreach (QPushButton *btn, btns) {
        connect(btn, SIGNAL(clicked()), this, SLOT(btnClicked()));
    }

    //绑定全局改变焦点信号槽
    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)), this, SLOT(focusChanged(QWidget *, QWidget *)));

    //绑定按键事件过滤器
    qApp->installEventFilter(this);
}

void frmInput2018::initNumber()
{
    //将按钮按下关联到对应的按钮
    connect(ui->btnNumber0, SIGNAL(clicked(bool)), ui->btn0, SLOT(click()));
    connect(ui->btnNumber1, SIGNAL(clicked(bool)), ui->btn1, SLOT(click()));
    connect(ui->btnNumber2, SIGNAL(clicked(bool)), ui->btn2, SLOT(click()));
    connect(ui->btnNumber3, SIGNAL(clicked(bool)), ui->btn3, SLOT(click()));
    connect(ui->btnNumber4, SIGNAL(clicked(bool)), ui->btn4, SLOT(click()));
    connect(ui->btnNumber5, SIGNAL(clicked(bool)), ui->btn5, SLOT(click()));
    connect(ui->btnNumber6, SIGNAL(clicked(bool)), ui->btn6, SLOT(click()));
    connect(ui->btnNumber7, SIGNAL(clicked(bool)), ui->btn7, SLOT(click()));
    connect(ui->btnNumber8, SIGNAL(clicked(bool)), ui->btn8, SLOT(click()));
    connect(ui->btnNumber9, SIGNAL(clicked(bool)), ui->btn9, SLOT(click()));
    connect(ui->btnNumberDot, SIGNAL(clicked(bool)), ui->btnDot, SLOT(click()));
    connect(ui->btnNumberDelete, SIGNAL(clicked(bool)), ui->btnDelete, SLOT(click()));
    connect(ui->btnNumberSpace, SIGNAL(clicked(bool)), ui->btnSpace, SLOT(click()));
    connect(ui->btnNumberClose, SIGNAL(clicked(bool)), ui->btnClose, SLOT(click()));
    connect(ui->btnNumberEnter, SIGNAL(clicked(bool)), ui->btnEnter, SLOT(click()));
}

void frmInput2018::initDb()
{
    py.open(dbPath);
}

void frmInput2018::btnClicked()
{
    //如果当前焦点控件类型为空,则返回不需要继续处理
    if (currentType.isEmpty()) {
        return;
    }

    QString labText = ui->labPY->text();
    QPushButton *btn = (QPushButton *)sender();
    QString objectName = btn->objectName();

    if (objectName == "btnType") {
        if (inputType == "english") {
            setInputType("chinese");
        } else if (inputType == "chinese") {
            if (mini) {
                setInputType("english");
            } else {
                setInputType("char");
            }
        } else if (inputType == "char") {
            setInputType("english");
        }

        upper = false;
        setUpper(upper);
    } else if (objectName == "btnUpper") {
        //大小写模式都要切换到英文状态
        setInputType("english");

        upper = !upper;
        setUpper(upper);
    } else if (objectName == "btnDelete") {
        //如果当前是中文模式,则删除对应拼音,删除完拼音之后再删除对应文本输入框的内容
        if (inputType == "chinese") {
            QString txt = labText;
            int len = txt.length();

            if (len > 0) {
                ui->labPY->setText(txt.left(len - 1));
                selectChinese();
            } else {
                deleteValue();
            }
        } else {
            deleteValue();
        }
    } else if (objectName == "btnPre") {
        if (currentPYIndex >= 10) {
            //每次最多显示 5 个汉字,所以每次向前的时候索引要减 5 * 2
            if (currentPYIndex % 5 == 0) {
                currentPYIndex -= 10;
            } else {
                currentPYIndex = currentPYCount - (currentPYCount % 5) - 5;
            }
        } else {
            currentPYIndex = 0;
        }

        showChinese();
    } else if (objectName == "btnNext") {
        if (currentPYIndex < currentPYCount - 1) {
            showChinese();
        }
    } else if (objectName == "btnClose") {
        hidePanel();
        clearChinese();
        ui->labPY->setText("");
    } else if (objectName == "btnSpace") {
        //如果中文模式而且有待输入字母,判断是否有中文则插入第一个中文否则插入字母
        if (inputType == "chinese" && labText != "") {
            if (ui->labCh1->text().isEmpty()) {
                insertValue(labText);
            } else {
                setChinese(0);
            }
        } else {
            insertValue(" ");
        }
    } else if (objectName == "btnEnter") {
        //如果中文模式而且有待输入字母则立即插入字母
        if (inputType == "chinese" && labText != "") {
            insertValue(labText);
        }

        if (currentLineEdit != 0) {
            hidePanel();
            QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier, QString("\n"));
            QApplication::sendEvent(currentLineEdit, &keyPress);
        } else {
            insertValue("\n");
        }
    } else {
        QString value = btn->text();

        //如果是&按钮，因为对应&被过滤,所以真实的text为去除前面一个&字符
        if (value == "&&") {
            value = "&";
        }

        //当前不是中文模式,则单击按钮对应text为传递参数
        if (inputType != "chinese") {
            insertValue(value);
        } else {
            //中文模式下,不允许输入特殊字符,单击对应数字按键取得当前索引的汉字
            if (btn->property("btnOther").toBool()) {
                if (labText.length() == 0) {
                    insertValue(value);
                }
            } else if (btn->property("btnNum").toBool()) {
                if (labText.length() == 0) {
                    insertValue(value);
                } else if (objectName == "btn1") {
                    setChinese(0);
                } else if (objectName == "btn2") {
                    setChinese(1);
                } else if (objectName == "btn3") {
                    setChinese(2);
                } else if (objectName == "btn4") {
                    setChinese(3);
                } else if (objectName == "btn5") {
                    setChinese(4);
                }
            } else if (btn->property("btnLetter").toBool()) {
                ui->labPY->setText(labText + value);
                selectChinese();
            }
        }
    }
}

void frmInput2018::focusChanged(QWidget *oldWidget, QWidget *nowWidget)
{
    //qDebug() << "oldWidget:" << oldWidget << "nowWidget:" << nowWidget;
    this->currentWidget = nowWidget;
    if (nowWidget != 0 && !this->isAncestorOf(nowWidget)) {
        //在Qt5和linux系统中(嵌入式linux除外),当输入法面板关闭时,焦点会变成无,然后焦点会再次移到焦点控件处
        //这样导致输入法面板的关闭按钮不起作用,关闭后马上有控件获取焦点又显示.
        //为此,增加判断,当焦点是从有对象转为无对象再转为有对象时不要显示.
        //这里又要多一个判断,万一首个窗体的第一个焦点就是落在可输入的对象中,则要过滤掉
        static bool isFirst = true;
#ifndef __arm__
        if (oldWidget == 0x0 && !isFirst) {
            QTimer::singleShot(0, this, SLOT(hidePanel()));
            return;
        }
#endif

        //如果对应属性noinput为真则不显示
        if (nowWidget->property("noinput").toBool() || nowWidget->property("readOnly").toBool()) {
            currentType.clear();
            QTimer::singleShot(0, this, SLOT(hidePanel()));
            return;
        }

        isFirst = false;
        currentFlag = nowWidget->property("flag").toString();

        if (nowWidget->inherits("QLineEdit")) {
            currentLineEdit = (QLineEdit *)nowWidget;
            if (!currentLineEdit->isReadOnly()) {
                currentType = "QLineEdit";
                showPanel();
            } else {
                currentType = "";
                hidePanel();
            }
        } else if (nowWidget->inherits("QTextEdit")) {
            currentTextEdit = (QTextEdit *)nowWidget;
            if (!currentTextEdit->isReadOnly()) {
                currentType = "QTextEdit";
                showPanel();
            } else {
                currentType = "";
                hidePanel();
            }
        } else if (nowWidget->inherits("QPlainTextEdit")) {
            currentPlain = (QPlainTextEdit *)nowWidget;
            if (!currentPlain->isReadOnly()) {
                currentType = "QPlainTextEdit";
                showPanel();
            } else {
                currentType = "";
                hidePanel();
            }
        } else if (nowWidget->inherits("QTextBrowser")) {
            currentBrowser = (QTextBrowser *)nowWidget;
            if (!currentBrowser->isReadOnly()) {
                currentType = "QTextBrowser";
                showPanel();
            } else {
                currentType = "";
                hidePanel();
            }
        } else if (nowWidget->inherits("QComboBox")) {
            //只有当下拉选择框处于编辑模式才可以输入
            QComboBox *cbox = (QComboBox *)nowWidget;
            if (cbox->isEditable()) {
                currentLineEdit = cbox->lineEdit() ;
                currentType = "QLineEdit";
                showPanel();
            } else {
                currentType = "";
                hidePanel();
            }
        } else if (nowWidget->inherits("QWidget")) {
            if (nowWidget->inherits("QAbstractSpinBox") || nowWidget->inherits("QWebView") || nowWidget->inherits("QtWebEngineCore::RenderWidgetHostViewQtDelegateWidget")) {
                currentType = "QWidget";
                showPanel();
            } else {
                currentType = "";
                hidePanel();
            }
        } else {
            //需要将输入法切换到最初的原始状态--小写,同时将之前的对象指针置为零
            currentWidget = 0;
            currentLineEdit = 0;
            currentTextEdit = 0;
            currentPlain = 0;
            currentBrowser = 0;
            currentType = "";

            setInputType("english");
            hidePanel();
        }

        movePosition();
    }
}

void frmInput2018::movePosition()
{
    if (currentType.isEmpty()) {
        return;
    }

    //根据用户选择的输入法位置设置-居中显示-底部填充-显示在输入框正下方
    int width = this->width();
    int height = this->height();
    if (number) {
        width = frmWidth / 2.5;
        height = frmHeight - topHeight;
    }

    if (position == "center") {
        QPoint pos = QPoint(deskWidth / 2 - width / 2, deskHeight / 2 - height / 2);
        this->setGeometry(pos.x(), pos.y(), width, height);
    } else if (position == "bottom") {
        this->setGeometry(0, deskHeight - height, deskWidth, height);
    } else if (position == "control") {
        QRect rect = currentWidget->rect();
        QPoint pos = QPoint(rect.left(), rect.bottom() + 2);
        pos = currentWidget->mapToGlobal(pos);

        int x = pos.x();
        if (x + width > deskWidth) {
            x = deskWidth - width;
        }

        int y = pos.y();
        if (y + height > deskHeight) {
            y = y - height - rect.height() - 2;
        }

        this->setGeometry(x, y, width, height);
    }
}

void frmInput2018::selectChinese()
{
    //清空汉字
    clearChinese();

    QString currentPY = ui->labPY->text();
    int count = py.select(currentPY);
    QStringList list;
    for (int i = 0; i < count; i++) {
        QString str = py.getChinese(i);
        list << str;
    }

    foreach (QString txt, list) {
        if (txt.length() > 0) {
            allPY.append(txt);
            currentPYCount++;
        }
    }

    //显示汉字
    showChinese();
    //自动计算汉字长度隐藏多余标签
    checkChinese();
}

void frmInput2018::showChinese()
{
    //每个版面最多显示 chineseCount 个汉字
    int count = 0;
    currentPY.clear();

    for (int i = 0; i < labCh.count(); i++) {
        labCh.at(i)->setText("");
    }

    for (int i = currentPYIndex; i < currentPYCount; i++) {
        if (count == 5) {
            break;
        }

        QString txt = QString("%1. %2").arg(count + 1).arg(allPY.at(currentPYIndex));
        currentPY.append(allPY.at(currentPYIndex));
        labCh.at(count)->setText(txt);
        count++;
        currentPYIndex++;
    }

    //qDebug() << "currentPYIndex:" << currentPYIndex << "currentPYCount:" << currentPYCount;
}

void frmInput2018::checkChinese()
{
    //根据当前汉字长度自动隐藏标签
    int len = ui->labCh1->text().length();
    if (len > 16) {
        ui->labCh1->setVisible(true);
        ui->labCh2->setVisible(false);
        ui->labCh3->setVisible(false);
        ui->labCh4->setVisible(false);
        ui->labCh5->setVisible(false);
    } else if (len > 12) {
        ui->labCh1->setVisible(true);
        ui->labCh2->setVisible(true);
        ui->labCh3->setVisible(false);
        ui->labCh4->setVisible(false);
        ui->labCh5->setVisible(false);
    } else if (len > 8) {
        ui->labCh1->setVisible(true);
        ui->labCh2->setVisible(true);
        ui->labCh3->setVisible(true);
        ui->labCh4->setVisible(false);
        ui->labCh5->setVisible(false);
    } else if (len > 4) {
        ui->labCh1->setVisible(true);
        ui->labCh2->setVisible(true);
        ui->labCh3->setVisible(true);
        ui->labCh4->setVisible(true);
        ui->labCh5->setVisible(false);
    } else {
        ui->labCh1->setVisible(true);
        ui->labCh2->setVisible(true);
        ui->labCh3->setVisible(true);
        ui->labCh4->setVisible(true);
        ui->labCh5->setVisible(true);
    }
}

void frmInput2018::insertValue(const QString &v)
{
    if (currentType.isEmpty()) {
        return;
    }

    QString value = currentWidget->property("upper").toBool() ? v.toUpper() : v;
    if (currentType == "QLineEdit") {
        currentLineEdit->insert(value);
    } else if (currentType == "QTextEdit") {
        currentTextEdit->insertPlainText(value);
    } else if (currentType == "QPlainTextEdit") {
        currentPlain->insertPlainText(value);
    } else if (currentType == "QTextBrowser") {
        currentBrowser->insertPlainText(value);
    } else if (currentType == "QWidget") {
        QStringList list = value.split("");
        foreach (QString str, list) {
            if (!str.isEmpty()) {
                QKeyEvent keyPress(QEvent::KeyPress, 0, Qt::NoModifier, QString(str));
                QApplication::sendEvent(currentWidget, &keyPress);
            }
        }
    }

    //将所有汉字标签可见
    ui->labCh3->setVisible(true);
    ui->labCh4->setVisible(true);
    ui->labCh5->setVisible(true);
    ui->labPY->setText("");
}

void frmInput2018::deleteValue()
{
    if (currentType.isEmpty()) {
        return;
    }

    QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier, QString());
    QApplication::sendEvent(currentWidget, &keyPress);
}

void frmInput2018::setChinese(int index)
{
    int count = currentPY.count();
    if (count > index) {
        insertValue(currentPY.at(index));
        //添加完一个汉字后,清空当前汉字信息,等待重新输入
        clearChinese();
        ui->labPY->setText("");
    }
}

void frmInput2018::clearChinese()
{
    //清空汉字,重置索引
    for (int i = 0; i < labCh.count(); i++) {
        labCh.at(i)->setText("");
    }

    allPY.clear();
    currentPY.clear();
    currentPYIndex = 0;
    currentPYCount = 0;
}

void frmInput2018::setDbPath(const QString &dbPath)
{
    this->dbPath = dbPath;
}

void frmInput2018::setFontInfo(const QString &fontName, int btnFontSize, int labFontSize)
{
    QFont btnFont(fontName, btnFontSize);
    QFont labFont(fontName, labFontSize);

    QList<QPushButton *> btns = this->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setFont(btnFont);
    }

    QList<QLabel *> labs = ui->widgetTop->findChildren<QLabel *>();
    foreach (QLabel *lab, labs) {
        lab->setFont(labFont);
    }

    ui->btnPre->setFont(btnFont);
    ui->btnNext->setFont(btnFont);
}

void frmInput2018::setFrmSize(int frmWidth, int frmHeight)
{
    this->frmWidth = frmWidth;
    this->frmHeight = frmHeight;
    this->setMinimumSize(frmWidth, frmHeight);
}

void frmInput2018::setIconSize(int iconWidth, int iconHeight)
{
    this->iconWidth = iconWidth;
    this->iconHeight = iconHeight;
    QSize icoSize(iconWidth, iconHeight);
    QSize icoSize2(iconWidth - 5, iconHeight - 5);
    QSize icoSize3(iconWidth + 3, iconHeight + 3);

    ui->btnSpace->setIconSize(icoSize);
    ui->btnClose->setIconSize(icoSize);
    ui->btnType->setIconSize(icoSize);
    ui->btnEnter->setIconSize(icoSize2);
    ui->btnUpper->setIconSize(icoSize2);
    ui->btnDelete->setIconSize(icoSize3);

    ui->btnNumberSpace->setIconSize(icoSize);
    ui->btnNumberClose->setIconSize(icoSize);
    ui->btnNumberEnter->setIconSize(icoSize2);
    ui->btnNumberDelete->setIconSize(icoSize3);
}

void frmInput2018::setSpacing(int spacing)
{
    this->spacing = spacing;
    ui->widgetMain->layout()->setSpacing(spacing);
    ui->widgetPreNext->layout()->setSpacing(spacing);
    ui->widgetNumber->layout()->setSpacing(spacing);
}

void frmInput2018::setTopHeight(int topHeight)
{
    this->topHeight = topHeight;
    ui->widgetTop->setFixedHeight(topHeight);
}

void frmInput2018::setMini(bool mini)
{
    this->mini = mini;
    if (mini) {
        labStatus->setVisible(true);
        ui->widgetMain->setVisible(false);
        ui->widgetPreNext->setVisible(false);
        ui->widgetNumber->setVisible(false);
        ui->widgetTop->layout()->setContentsMargins(9, 9, 9, 9);
        ui->verticalSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    } else {
        labStatus->setVisible(false);
        ui->widgetMain->setVisible(true);
        ui->widgetPreNext->setVisible(true);
        ui->widgetTop->layout()->setContentsMargins(9, 9, 9, 0);
        ui->verticalSpacer->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    }
}

void frmInput2018::setPosition(const QString &position)
{
    this->position = position;
}

void frmInput2018::setStyle(const QString &style)
{
    QString iconType = "white";
    if (style == "blue") {
        setColor("#377FC9", "#F3F3F3", "#4189D3", "#386487", "#F3F3F3", "#386487", "#F3F3F3");
    } else if (style == "gray") {
        setColor("#989898", "#FFFFFF", "#A2A2A2", "#333333", "#F3F3F3", "#2E2E2E", "#F3F3F3");
    } else if (style == "black") {
        setColor("#191919", "#F3F3F3", "#313131", "#4189D3", "#F3F3F3", "#F95717", "#F3F3F3");
    } else if (style == "brown") {
        setColor("#667481", "#F3F3F3", "#566373", "#4189D3", "#F3F3F3", "#4189D3", "#F3F3F3");
    } else if (style == "silvery") {
        setColor("#868690", "#000002", "#C3C2C7", "#F0F0F0", "#000002", "#F0F0F0", "#000002");
        iconType = "black";
    }

    QString icon_upper = QString(":/image/upper_%1.png").arg(iconType);
    QString icon_delete = QString(":/image/delete_%1.png").arg(iconType);
    QString icon_enter = QString(":/image/enter_%1.png").arg(iconType);
    QString icon_close = QString(":/image/close_%1.png").arg(iconType);
    QString icon_space = QString(":/image/space_%1.png").arg(iconType);

    ui->btnSpace->setIcon(QIcon(icon_space));
    ui->btnClose->setIcon(QIcon(icon_close));
    ui->btnEnter->setIcon(QIcon(icon_enter));
    ui->btnUpper->setIcon(QIcon(icon_upper));
    ui->btnDelete->setIcon(QIcon(icon_delete));

    ui->btnNumberSpace->setIcon(QIcon(icon_space));
    ui->btnNumberClose->setIcon(QIcon(icon_close));
    ui->btnNumberEnter->setIcon(QIcon(icon_enter));
    ui->btnNumberDelete->setIcon(QIcon(icon_delete));
}

void frmInput2018::setColor(const QString &mainBackgroundColor, const QString &mainTextColor, const QString &btnBackgroundColor,
                            const QString &btnHoveColor, const QString &btnHoveTextColor, const QString &labHoveColor, const QString &labHoveTextColor)
{
    QStringList qss;
    qss.append(QString("QWidget#widgetPreNext{background:none;}"));
    qss.append(QString("QWidget#widgetMain,QWidget#widgetTop,QWidget#widgetNumber{background-color:%1;}").arg(mainBackgroundColor));
    qss.append(QString("QPushButton{border-width:0px;border-radius:3px;color:%1;}").arg(mainTextColor));
    qss.append(QString("QPushButton{padding:0px;background-color:%1;}").arg(btnBackgroundColor));
    //如果仅仅是触摸屏请用下面这句
    //qss.append(QString("QPushButton:pressed{background-color:%1;color:%2;}").arg(btnHoveColor).arg(btnHoveTextColor));
    qss.append(QString("QPushButton:pressed,QPushButton:hover{background-color:%1;color:%2;}").arg(btnHoveColor).arg(btnHoveTextColor));
    qss.append(QString("QLabel{border-width:0px;border-radius:3px;color:%1;background:none;}").arg(mainTextColor));
    qss.append(QString("QLabel:pressed,QLabel:hover{background-color:%1;color:%2;}").arg(labHoveColor).arg(labHoveTextColor));

    if (mini) {
        qss.append(QString("QWidget#widgetTop{border-radius:5px;}"));
    }

    this->setStyleSheet(qss.join(""));
    qApp->processEvents();
}

void frmInput2018::setInputType(const QString &inputType)
{
    this->inputType = inputType;
    if (inputType == "english") {
        ui->btnType->setText("En");
        ui->widgetTop->setEnabled(false);
        showChar(false);
        labStatus->setPixmap(QPixmap(":/image/status_en.png"));

        if (mini) {
            hide();
        }
    } else if (inputType == "chinese") {
        ui->btnType->setText("中");
        ui->widgetTop->setEnabled(true);
        showChar(false);
        labStatus->setPixmap(QPixmap(":/image/status_ch.png"));

        if (mini) {
            show();
        }
    } else if (inputType == "char") {
        ui->btnType->setText("符");
        ui->widgetTop->setEnabled(false);
        showChar(true);
    }

    //每次切换到模式,都要执行清空之前中文模式下的信息
    clearChinese();
    ui->labPY->setText("");
}

void frmInput2018::setUpper(bool isUpper)
{
    QList<QPushButton *> btn = ui->widgetMain->findChildren<QPushButton *>();
    foreach (QPushButton *b, btn) {
        if (b->property("btnLetter").toBool()) {
            QString text = isUpper ? b->text().toUpper() : b->text().toLower();
            b->setText(text);
        }
    }
}

void frmInput2018::showChar(bool isChar)
{
    QStringList chars;
    if (isChar) {
        chars << "!" << "@" << "#" << "$" << "%" << "^" << "&&" << "*" << "(" << ")";
        chars << "~" << "`" << "-" << "_" << "+" << "=" << "{" << "}" << "[" << "]";
        chars << "|" << "\\" << ":" << ";" << "\"" << "'" << "<" << ">" << "?";
        chars << "," << "/" << "：" << "，" << "。" << "？" << "；";
    } else {
        chars << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "0";
        chars << "q" << "w" << "e" << "r" << "t" << "y" << "u" << "i" << "o" << "p";
        chars << "a" << "s" << "d" << "f" << "g" << "h" << "j" << "k" << "l";
        chars << "z" << "x" << "c" << "v" << "b" << "n" << "m";
    }

    ui->btn1->setText(chars.at(0));
    ui->btn2->setText(chars.at(1));
    ui->btn3->setText(chars.at(2));
    ui->btn4->setText(chars.at(3));
    ui->btn5->setText(chars.at(4));
    ui->btn6->setText(chars.at(5));
    ui->btn7->setText(chars.at(6));
    ui->btn8->setText(chars.at(7));
    ui->btn9->setText(chars.at(8));
    ui->btn0->setText(chars.at(9));

    ui->btnq->setText(chars.at(10));
    ui->btnw->setText(chars.at(11));
    ui->btne->setText(chars.at(12));
    ui->btnr->setText(chars.at(13));
    ui->btnt->setText(chars.at(14));
    ui->btny->setText(chars.at(15));
    ui->btnu->setText(chars.at(16));
    ui->btni->setText(chars.at(17));
    ui->btno->setText(chars.at(18));
    ui->btnp->setText(chars.at(19));

    ui->btna->setText(chars.at(20));
    ui->btns->setText(chars.at(21));
    ui->btnd->setText(chars.at(22));
    ui->btnf->setText(chars.at(23));
    ui->btng->setText(chars.at(24));
    ui->btnh->setText(chars.at(25));
    ui->btnj->setText(chars.at(26));
    ui->btnk->setText(chars.at(27));
    ui->btnl->setText(chars.at(28));

    ui->btnz->setText(chars.at(29));
    ui->btnx->setText(chars.at(30));
    ui->btnc->setText(chars.at(31));
    ui->btnv->setText(chars.at(32));
    ui->btnb->setText(chars.at(33));
    ui->btnn->setText(chars.at(34));
    ui->btnm->setText(chars.at(35));
}

void frmInput2018::showPanel()
{
    if (!isEnabled()) {
        return;
    }

    if (mini && inputType == "english") {
        return;
    }

    //根据控件弱属性设置是否显示为数字小键盘
    currentFlag = currentWidget->property("flag").toString();
    if (currentWidget->inherits("QAbstractSpinBox")) {
        currentFlag = "number";
    }

    setMode(currentFlag == "number");

    this->setVisible(true);
    int width = ui->btna->width();
    width = width > 35 ? width : 35;
    ui->btnPre->setFixedWidth(width);
    ui->btnNext->setFixedWidth(width);
}

void frmInput2018::hidePanel()
{
    this->setVisible(false);
}

void frmInput2018::setMode(bool number)
{
    //只有键盘模式改变了才需要切换,否则会产生闪烁
    if (this->number != number && !mini) {
        this->number = number;

        //先隐藏下防止闪烁
        if (position != "bottom") {
            this->setVisible(false);
        }

        ui->widgetTop->setVisible(!number);
        ui->widgetMain->setVisible(!number);
        ui->widgetNumber->setVisible(number);
        setIconSize(iconWidth, iconHeight);

        //清空原有汉字
        clearChinese();
        ui->labPY->setText("");

        if (number) {
            setMinimumSize(frmWidth / 2.5, frmHeight - topHeight);
        } else {
            setMinimumSize(frmWidth, frmHeight);
        }
    }
}
