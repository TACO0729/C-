#ifndef QUIZPAGE_H
#define QUIZPAGE_H

#include <QWidget>
#include <QButtonGroup>

#include "mbtiengine.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QuizPage; }
QT_END_NAMESPACE

class QuizPage : public QWidget
{
    Q_OBJECT

public:
    explicit QuizPage(MBTIEngine* engine, QWidget* parent = nullptr);
    ~QuizPage();

    void restartQuiz();

signals:
    void quizFinished();
    void goBackToWelcome();

private slots:
    void on_btnPrev_clicked();
    void on_btnNext_clicked();

private:
    void loadQuestion(int index);
    void updateProgress();
    void updateButtonState();
    void saveCurrentAnswer();
    void restoreAnswer(int index);
    void clearRadioSelection();

    void applyQuizStyle();
    QString chooseNiceFontFamily() const;

    bool currentQuestionHasAnswer() const;
    void goToFirstUnansweredQuestion();

    QString emoji(uint codepoint) const;

private:
    Ui::QuizPage* ui;
    MBTIEngine* m_engine;

    QButtonGroup* m_answerGroup;
    int m_currentIndex;
};

#endif // QUIZPAGE_H