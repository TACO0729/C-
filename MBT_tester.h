#pragma once
#ifndef MBT_TESTER_H
#define MBT_TESTER_H

#include <QMainWindow>
#include "mbtiengine.h"
#include "quizpage.h"
#include "resultpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MBT_tester; }
QT_END_NAMESPACE

class MBT_tester : public QMainWindow
{
    Q_OBJECT

public:
    MBT_tester(QWidget* parent = nullptr);
    ~MBT_tester();

private slots:
    void on_btnStart_clicked();
    void onQuizFinished();
    void onRestartQuiz();

private:
    void showWelcome();
    void showQuiz();
    void showResult();
    void applyMainStyle();

private:
    Ui::MBT_tester* ui;
    MBTIEngine* m_engine;
    QuizPage* m_quizPage;
    ResultPage* m_resultPage;
};

#endif // MBT_TESTER_H