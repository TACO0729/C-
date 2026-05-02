#ifndef MBTIENGINE_H
#define MBTIENGINE_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QString>

struct Question {
    QString text;
    QString optionA;
    QString optionB;
    char dimension;      // 'E', 'S', 'T', 'J'
    bool isA_LeftSide;   // true: A=E/S/T/J, B=I/N/F/P
};

struct QuizResult {
    int scoreE_I = 0;
    int scoreS_N = 0;
    int scoreT_F = 0;
    int scoreJ_P = 0;

    QString getTypeCode() const;
    int getScore(char dim) const;
};

class MBTIEngine : public QObject
{
    Q_OBJECT

public:
    explicit MBTIEngine(QObject* parent = nullptr);

    bool loadQuestions(const QString& jsonPath);

    const QVector<Question>& questions() const { return m_questions; }
    int totalQuestions() const { return m_questions.size(); }

    void answerQuestion(int index, int choice);  // 1=A, 2=B, 3=Neutral
    int getAnswer(int index) const;

    bool isAnswered(int index) const;
    int answeredCount() const;
    bool allAnswered() const;
    int firstUnansweredIndex() const;

    QuizResult calculateResult() const;
    void reset();

    static QString getTypeNickname(const QString& typeCode);
    static QString getTypeDescription(const QString& typeCode);
    static QString getDimensionName(char dim);
    static QString getDimensionLeft(char dim);
    static QString getDimensionRight(char dim);

private:
    QVector<Question> m_questions;
    QVector<int> m_answers;

    static const QMap<QString, QString> s_nicknames;
    static const QMap<QString, QString> s_descriptions;
};

#endif // MBTIENGINE_H