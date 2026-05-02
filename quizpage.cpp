#include "quizpage.h"
#include "ui_quizpage.h"

#include <QFont>
#include <QFontDatabase>
#include <QSignalBlocker>
#include <QAbstractButton>

QuizPage::QuizPage(MBTIEngine* engine, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::QuizPage)
    , m_engine(engine)
    , m_answerGroup(nullptr)
    , m_currentIndex(0)
{
    ui->setupUi(this);

    m_answerGroup = new QButtonGroup(this);
    m_answerGroup->addButton(ui->radioA, 1);
    m_answerGroup->addButton(ui->radioB, 2);
    m_answerGroup->addButton(ui->radioNeutral, 3);
    m_answerGroup->setExclusive(true);

    applyQuizStyle();

    connect(ui->btnPrev, &QPushButton::clicked, this, &QuizPage::on_btnPrev_clicked);
    connect(ui->btnNext, &QPushButton::clicked, this, &QuizPage::on_btnNext_clicked);

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    connect(m_answerGroup, &QButtonGroup::idClicked, this, [this](int id) {
        if (!m_engine) {
            return;
        }

        if (id == 1 || id == 2 || id == 3) {
            m_engine->answerQuestion(m_currentIndex, id);
            updateProgress();
            updateButtonState();
        }
        });
#else
    connect(m_answerGroup, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
        this, [this](int id) {
            if (!m_engine) {
                return;
            }

            if (id == 1 || id == 2 || id == 3) {
                m_engine->answerQuestion(m_currentIndex, id);
                updateProgress();
                updateButtonState();
            }
        });
#endif

    connect(ui->radioA, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked && m_engine) {
            m_engine->answerQuestion(m_currentIndex, 1);
            updateProgress();
            updateButtonState();
        }
        });

    connect(ui->radioB, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked && m_engine) {
            m_engine->answerQuestion(m_currentIndex, 2);
            updateProgress();
            updateButtonState();
        }
        });

    connect(ui->radioNeutral, &QRadioButton::toggled, this, [this](bool checked) {
        if (checked && m_engine) {
            m_engine->answerQuestion(m_currentIndex, 3);
            updateProgress();
            updateButtonState();
        }
        });

    loadQuestion(0);
}

QuizPage::~QuizPage()
{
    delete ui;
}

void QuizPage::restartQuiz()
{
    m_currentIndex = 0;
    loadQuestion(0);
}

QString QuizPage::emoji(uint codepoint) const
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    char32_t ucs4[1] = { static_cast<char32_t>(codepoint) };
    return QString::fromUcs4(ucs4, 1);
#else
    uint ucs4[1] = { codepoint };
    return QString::fromUcs4(ucs4, 1);
#endif
}

QString QuizPage::chooseNiceFontFamily() const
{
    const QStringList families = QFontDatabase::families();

    if (families.contains("Microsoft YaHei UI")) {
        return "Microsoft YaHei UI";
    }

    if (families.contains("Microsoft YaHei")) {
        return "Microsoft YaHei";
    }

    if (families.contains("SimHei")) {
        return "SimHei";
    }

    if (families.contains("Arial")) {
        return "Arial";
    }

    return font().family();
}

void QuizPage::applyQuizStyle()
{
    const QString fontFamily = chooseNiceFontFamily();

    QFont baseFont(fontFamily);
    baseFont.setPointSize(12);
    setFont(baseFont);

    setAutoFillBackground(true);

    ui->labelQuestionNum->setAlignment(Qt::AlignCenter);
    ui->labelProgress->setAlignment(Qt::AlignCenter);
    ui->labelQuestionText->setAlignment(Qt::AlignCenter);
    ui->labelQuestionText->setWordWrap(true);

    ui->radioA->setCursor(Qt::PointingHandCursor);
    ui->radioB->setCursor(Qt::PointingHandCursor);
    ui->radioNeutral->setCursor(Qt::PointingHandCursor);
    ui->btnPrev->setCursor(Qt::PointingHandCursor);
    ui->btnNext->setCursor(Qt::PointingHandCursor);

    ui->progressBar->setTextVisible(false);

    ui->radioA->setMinimumHeight(76);
    ui->radioB->setMinimumHeight(76);
    ui->radioNeutral->setMinimumHeight(76);

    setStyleSheet(QString(R"(
        QWidget {
            background-color: qlineargradient(
                x1:0, y1:0, x2:1, y2:1,
                stop:0 #fff7fb,
                stop:0.28 #f7f1ff,
                stop:0.62 #eef7ff,
                stop:1 #fffbea
            );
            color: #222222;
            font-family: "%1";
        }

        QLabel#labelQuestionNum {
            background-color: rgba(255, 255, 255, 225);
            color: #241d38;
            font-size: 31px;
            font-weight: 700;
            padding-top: 18px;
            padding-bottom: 14px;
            border-bottom: 1px solid rgba(128, 104, 179, 70);
        }

        QLabel#labelProgress {
            background-color: rgba(255, 255, 255, 195);
            color: #8068b3;
            font-size: 18px;
            font-weight: 500;
            padding-top: 10px;
            padding-bottom: 12px;
            border-bottom: 1px solid rgba(128, 104, 179, 55);
        }

        QLabel#labelQuestionText {
            color: #23202e;
            font-size: 30px;
            font-weight: 600;
            background-color: rgba(255, 255, 255, 245);
            border: 1px solid rgba(128, 104, 179, 95);
            border-top: 12px solid #8068b3;
            border-radius: 26px;
            padding: 38px 42px;
            margin-left: 44px;
            margin-right: 44px;
            margin-top: 24px;
            margin-bottom: 26px;
        }

        QProgressBar {
            height: 22px;
            border: none;
            border-radius: 11px;
            background-color: rgba(225, 217, 239, 230);
            margin-left: 60px;
            margin-right: 60px;
            margin-top: 10px;
            margin-bottom: 10px;
        }

        QProgressBar::chunk {
            border-radius: 11px;
            background-color: qlineargradient(
                x1:0, y1:0, x2:1, y2:0,
                stop:0 #8068b3,
                stop:0.45 #a884d6,
                stop:1 #f0b46c
            );
        }

        QRadioButton {
            color: #241f30;
            background-color: rgba(255, 255, 255, 248);
            border: 2px solid rgba(128, 104, 179, 105);
            border-radius: 22px;

            font-size: 23px;
            font-weight: 500;

            min-height: 62px;
            padding-left: 26px;
            padding-right: 26px;
            padding-top: 8px;
            padding-bottom: 8px;

            margin-left: 72px;
            margin-right: 72px;
            margin-top: 8px;
            margin-bottom: 8px;

            spacing: 18px;
        }

        QRadioButton:hover {
            background-color: #fff8ec;
            border: 2px solid #f0b46c;
            color: #4d3978;
        }

        QRadioButton:checked {
            background-color: #f1eafa;
            border: 3px solid #8068b3;
            color: #4d3978;
            font-weight: 700;
        }

        QRadioButton::indicator {
            width: 26px;
            height: 26px;
        }

        QPushButton {
            min-width: 170px;
            min-height: 54px;
            color: #ffffff;
            background-color: #8068b3;
            border: none;
            border-radius: 18px;
            font-size: 22px;
            font-weight: 700;
            padding: 10px 34px;
            margin: 16px 22px 20px 22px;
        }

        QPushButton:hover {
            background-color: #6f58a5;
        }

        QPushButton:pressed {
            background-color: #5c478d;
        }

        QPushButton:disabled {
            background-color: #c9bfdc;
            color: #ffffff;
        }

        QPushButton#btnPrev {
            background-color: #a995d0;
        }

        QPushButton#btnPrev:hover {
            background-color: #927fbd;
        }

        QPushButton#btnPrev:disabled {
            background-color: #ddd5eb;
            color: #ffffff;
        }

        QPushButton#btnNext {
            background-color: qlineargradient(
                x1:0, y1:0, x2:1, y2:0,
                stop:0 #8068b3,
                stop:1 #f0a84f
            );
        }

        QPushButton#btnNext:hover {
            background-color: qlineargradient(
                x1:0, y1:0, x2:1, y2:0,
                stop:0 #6f58a5,
                stop:1 #e69b3f
            );
        }

        QPushButton#btnNext:disabled {
            background-color: #cfc4e4;
            color: #ffffff;
        }
    )").arg(fontFamily));
}

void QuizPage::loadQuestion(int index)
{
    if (!m_engine || index < 0 || index >= m_engine->totalQuestions()) {
        return;
    }

    m_currentIndex = index;

    const Question& q = m_engine->questions()[index];
    const int total = m_engine->totalQuestions();

    ui->labelQuestionNum->setText(
        QString("%1   问题  %2  /  %3   %4")
        .arg(emoji(0x1F338))
        .arg(index + 1)
        .arg(total)
        .arg(emoji(0x1F338))
    );

    ui->labelQuestionText->setText(
        QString("\"%1\"").arg(q.text)
    );

    QString optionAText = q.optionA.trimmed();
    QString optionBText = q.optionB.trimmed();

    if (optionAText.isEmpty()) {
        optionAText = "选项 A";
    }

    if (optionBText.isEmpty()) {
        optionBText = "选项 B";
    }

    ui->radioA->setText("A.  " + optionAText);
    ui->radioB->setText("B.  " + optionBText);
    ui->radioNeutral->setText("C.  两者差不多 / 暂时难以选择");

    ui->radioA->setToolTip(ui->radioA->text());
    ui->radioB->setToolTip(ui->radioB->text());
    ui->radioNeutral->setToolTip(ui->radioNeutral->text());

    clearRadioSelection();
    restoreAnswer(index);

    updateProgress();
    updateButtonState();
}

void QuizPage::clearRadioSelection()
{
    if (!m_answerGroup) {
        return;
    }

    QSignalBlocker blockerGroup(m_answerGroup);
    QSignalBlocker blockerA(ui->radioA);
    QSignalBlocker blockerB(ui->radioB);
    QSignalBlocker blockerNeutral(ui->radioNeutral);

    const bool oldExclusive = m_answerGroup->exclusive();

    m_answerGroup->setExclusive(false);

    ui->radioA->setChecked(false);
    ui->radioB->setChecked(false);
    ui->radioNeutral->setChecked(false);

    m_answerGroup->setExclusive(oldExclusive);
}

void QuizPage::restoreAnswer(int index)
{
    if (!m_engine) {
        return;
    }

    const int ans = m_engine->getAnswer(index);

    QSignalBlocker blockerGroup(m_answerGroup);
    QSignalBlocker blockerA(ui->radioA);
    QSignalBlocker blockerB(ui->radioB);
    QSignalBlocker blockerNeutral(ui->radioNeutral);

    if (ans == 1) {
        ui->radioA->setChecked(true);
    }
    else if (ans == 2) {
        ui->radioB->setChecked(true);
    }
    else if (ans == 3) {
        ui->radioNeutral->setChecked(true);
    }
}

bool QuizPage::currentQuestionHasAnswer() const
{
    if (!m_engine) {
        return false;
    }

    return m_engine->isAnswered(m_currentIndex);
}

void QuizPage::updateProgress()
{
    if (!m_engine) {
        return;
    }

    const int total = m_engine->totalQuestions();
    const int answered = m_engine->answeredCount();

    ui->progressBar->setRange(0, total);
    ui->progressBar->setValue(m_currentIndex + 1);

    int pagePercent = 0;
    if (total > 0) {
        pagePercent = static_cast<int>(((m_currentIndex + 1) * 100.0 / total) + 0.5);
    }

    ui->labelProgress->setText(
        QString("当前进度：%1%        已回答：%2 / %3")
        .arg(pagePercent)
        .arg(answered)
        .arg(total)
    );
}

void QuizPage::updateButtonState()
{
    if (!m_engine) {
        return;
    }

    const int total = m_engine->totalQuestions();

    ui->btnPrev->setEnabled(m_currentIndex > 0);

    if (m_currentIndex == total - 1) {
        ui->btnNext->setText("查看结果");
    }
    else {
        ui->btnNext->setText("下一题");
    }

    ui->btnNext->setEnabled(currentQuestionHasAnswer());
}

void QuizPage::saveCurrentAnswer()
{
    if (!m_engine || !m_answerGroup) {
        return;
    }

    int checkedId = m_answerGroup->checkedId();

    if (ui->radioA->isChecked()) {
        checkedId = 1;
    }
    else if (ui->radioB->isChecked()) {
        checkedId = 2;
    }
    else if (ui->radioNeutral->isChecked()) {
        checkedId = 3;
    }

    if (checkedId == 1 || checkedId == 2 || checkedId == 3) {
        m_engine->answerQuestion(m_currentIndex, checkedId);
    }
}

void QuizPage::on_btnPrev_clicked()
{
    saveCurrentAnswer();

    if (m_currentIndex > 0) {
        loadQuestion(m_currentIndex - 1);
    }
}

void QuizPage::goToFirstUnansweredQuestion()
{
    if (!m_engine) {
        return;
    }

    const int firstIndex = m_engine->firstUnansweredIndex();

    if (firstIndex >= 0) {
        loadQuestion(firstIndex);
    }
}

void QuizPage::on_btnNext_clicked()
{
    saveCurrentAnswer();

    if (!m_engine) {
        return;
    }

    const int total = m_engine->totalQuestions();

    if (!currentQuestionHasAnswer()) {
        updateButtonState();
        return;
    }

    if (m_currentIndex < total - 1) {
        loadQuestion(m_currentIndex + 1);
        return;
    }

    if (!m_engine->allAnswered()) {
        goToFirstUnansweredQuestion();
        return;
    }

    emit quizFinished();
}