# Decision-trees-and-Naive-Bayes-Model

This was a really cool AI course porject.
The full specifications can be found in asst3.pdf

Classification Problem: We want to classify the topic of some articles based on the words that appear in them.

First approach is to train a decision tree where the splits are based on the information gain of each word. You can find the implementation of the decision tree in Main.cc

The second approach is to use a Naive Bayes Network where we assume that the existance of each word in an article is independent of the existance of any other word in that article, even though this assumption is incorrect we still get very accurate results on the test set. You can find the implementation of the Naive Bayes Network in Main2.cc

