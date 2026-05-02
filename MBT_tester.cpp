#include "MBT_tester.h"
#include "ui_MBT_tester.h"

#include <QMessageBox>

MBT_tester::MBT_tester(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MBT_tester)
    , m_engine(new MBTIEngine(this))
{
    ui->setupUi(this);

    setWindowTitle("MBTI 人格测试");
    resize(1100, 720);
    setMinimumSize(980, 650);

    applyMainStyle();

    QString jsonPath = ":/questions.json";
    if (!m_engine->loadQuestions(jsonPath)) {
        ui->labelWelcome->setText("错误：无法加载题库文件 questions.json\n请检查资源文件配置");
        ui->btnStart->setEnabled(false);
    }

    m_quizPage = new QuizPage(m_engine, this);
    m_resultPage = new ResultPage(this);

    ui->stackedWidget->addWidget(m_quizPage);
    ui->stackedWidget->addWidget(m_resultPage);

    connect(ui->btnStart, &QPushButton::clicked, this, &MBT_tester::on_btnStart_clicked);
    connect(m_quizPage, &QuizPage::quizFinished, this, &MBT_tester::onQuizFinished);
    connect(m_resultPage, &ResultPage::restartQuiz, this, &MBT_tester::onRestartQuiz);

    ui->stackedWidget->setCurrentIndex(0);
}

MBT_tester::~MBT_tester()
{
    delete ui;
}

void MBT_tester::applyMainStyle()
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f8f4ff;
        }

        QStackedWidget {
            background-color: #f8f4ff;
            border: none;
        }

        QLabel {
            color: #222222;
        }

        QPushButton {
            min-height: 42px;
            border-radius: 14px;
            background-color: #8068b3;
            color: white;
            font-size: 18px;
            padding: 8px 24px;
        }

        QPushButton:hover {
            background-color: #6f58a5;
        }

        QPushButton:pressed {
            background-color: #5d488f;
        }

        QPushButton:disabled {
            background-color: #cfc4e4;
            color: #ffffff;
        }
    )");
}

void MBT_tester::on_btnStart_clicked()
{
    m_engine->reset();

    if (m_quizPage) {
        m_quizPage->restartQuiz();
    }

    showQuiz();
}

void MBT_tester::onQuizFinished()
{
    QuizResult result = m_engine->calculateResult();
    m_resultPage->setResult(result);
    showResult();
}

void MBT_tester::onRestartQuiz()
{
    m_engine->reset();

    if (m_quizPage) {
        m_quizPage->restartQuiz();
    }

    showWelcome();
}

void MBT_tester::showWelcome()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MBT_tester::showQuiz()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MBT_tester::showResult()
{
    ui->stackedWidget->setCurrentIndex(2);
}