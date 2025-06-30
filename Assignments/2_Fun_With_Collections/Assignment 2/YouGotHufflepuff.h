#ifndef YouGotHufflepuff_Included
#define YouGotHufflepuff_Included

#include "map.h"
#include "set.h"
#include <string>
#include <iostream>

/* Type: Question
 *
 * Type representing a personality quiz question.
 */
struct Question {
    std::string questionText; // Text of the question
    Map<char, int> factors;   // Map from factors to +1 or -1
};

/* Type: Person
 *
 * Type representing a person, used to represent people when determining
 * who's the closest match to the user.
 */
struct Person {
    std::string name;      // Name of the person
    Map<char, int> scores; // Map from factors to +1 or -1
};

/**
 * Removes and returns a randomly-chosen question from the given set of unasked
 * questions. If the set of questions is empty, this function calls error() to
 * report an error.
 *
 * @param unaskedQuestions The set of unasked questions.
 * @return A randomly-chosen question from the set.
 */
Question randomQuestionFrom(Set<Question>& unaskedQuestions);

/**
 * Given a user's answers to a set of personality questions, returns their
 * factor scores based on those answers.
 *
 * @param answers A map from the administered questions to the answers given.
 * @return The scores for the user.
 */
Map<char, int> scoresFrom(const Map<Question, int>& answers);

/**
 * Normalizes the scores in the given Map by dividing each by the square root
 * of the sum of the squares of the values in the map. (Mathematically, this
 * returns a unit vector pointing in the direction given by the scores vector.)
 * <p>
 * If there are no nonzero entries in the scores map, this function reports an
 * error by calling error().
 *
 * @param scores A collection of scores.
 * @return A normalized version of those scores.
 */
Map<char, double> normalize(const Map<char, int>& scores);

/**
 * Returns the cosine similarity of the two normalized sets of scores. This
 * value ranges from -1 (diametrically opposite) to +1 (perfectly similar).
 *
 * @param lhs The first set of normalized scores.
 * @param rhs The second set of normalized scores.
 * @return Their cosine similarity.
 */
double cosineSimilarityOf(const Map<char, double>& lhs,
                          const Map<char, double>& rhs);

/**
 * Given a set of scores and a group of people, returns the person in the group whose
 * scores have the highest cosine similarity with the the initial set of scores.
 * <p>
 * If the input scores cannot be normalized, or if any of the people in the Set have
 * scores that cannot be normalized, this function reports an error with error().
 * <p>
 * If there are no people in the set, this function calls error().
 *
 * @param scores The user's scores.
 * @param people The list of fictional people they can be similar to.
 * @return The person most similar to the user, measured by cosine similarity.
 */
Person mostSimilarTo(const Map<char, int>& scores, const Set<Person>& people);





/* Everything below this point is designed to make C++ play nicely with
 * the custom types we defined here. Feel free to check these out if you're
 * curious, and take CS106L if you want to learn more!
 */

/* Functions to read or write questions and people to/from a stream. */
std::istream& operator>> (std::istream& in, Question& toRead);
std::istream& operator>> (std::istream& in, Person& toRead);
std::ostream& operator<< (std::ostream& out, const Question& toWrite);
std::ostream& operator<< (std::ostream& out, const Person& toWrite);

/* Relational operators for questions and people; used internally by the Map and Set. */
bool operator< (const Question& lhs, const Question& rhs);
bool operator< (const Person& lhs,   const Person& rhs);

/* Comparison operators for questions and people; used by the test driver. */
bool operator== (const Question& lhs, const Question& rhs);
bool operator!= (const Question& lhs, const Question& rhs);

bool operator== (const Person& lhs, const Person& rhs);
bool operator!= (const Person& lhs, const Person& rhs);

#endif
