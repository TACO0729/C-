#ifndef RESULTPAGE_H
#define RESULTPAGE_H

#include <QWidget>
#include <QFrame>
#include "mbtiengine.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class ResultPage;
}
QT_END_NAMESPACE

class DimensionBar : public QFrame
{
    Q_OBJECT

public:
    explicit DimensionBar(char dim, int score, int maxScore, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString preferredFontFamily() const;

private:
    char m_dim;
    int m_score;
    int m_maxScore;
};

class ResultPage : public QWidget
{
    Q_OBJECT

public:
    explicit ResultPage(QWidget* parent = nullptr);
    ~ResultPage();

    void setResult(const QuizResult& result);

signals:
    void restartQuiz();

private slots:
    void on_btnRestart_clicked();

private:
    QString preferredFontFamily() const;
    QString colorForType(const QString& typeCode) const;

private:
    Ui::ResultPage* ui;
};

#endif // RESULTPAGE_H