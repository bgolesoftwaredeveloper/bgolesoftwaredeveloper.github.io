// -----------------------------------------------------------------------------
// Package: naivebayesclassifier
// File: naive_bayes_classifier_test.go
//
// Description:
//
//	This file contains unit tests for the Multinomial Naive Bayes text classifier algorithm.
//	The test suite validates basic initialization, defensive input validation, single and
//	multi-class training, tokenization, frequency accumulation, Laplace smoothing, and log-posterior
//	inference across various categorical text domains.
//
//	The test suite focuses on:
//
//	  - Basic instantiation and structure invariants.
//	  - Defensive execution against nil receivers, empty texts, blank classes, and unseen words.
//	  - Exact tokenization behavior (lowercasing, whitespace trimming, empty string handling).
//	  - Frequency count updates for classes, vocabulary, features, and document totals.
//	  - Sentiment classification, spam detection, and multi-class categorization accuracy.
//	  - Boundary behavior with repeated training, single-token documents, and uppercase text inputs.
//
// Notes:
//   - All tests use explicit Arrange / Act / Assert structure.
//   - The testing parameter is intentionally named "test" for clarity.
//   - Descriptive variable names are used throughout.
//
// -----------------------------------------------------------------------------
package naivebayesclassifier

import (
	"reflect"
	"testing"
)

// verifyClassifierInvariants validates internal map allocations and count non-negativity.
func verifyClassifierInvariants(test *testing.T, classifier *Classifier) {
	test.Helper()

	if classifier == nil {
		test.Fatal("invariant violation: classifier instance is nil")
	}

	if classifier.classes == nil {
		test.Fatal("invariant violation: classes map is nil")
	}

	if classifier.features == nil {
		test.Fatal("invariant violation: features map is nil")
	}

	if classifier.classSize == nil {
		test.Fatal("invariant violation: classSize map is nil")
	}

	if classifier.vocabulary == nil {
		test.Fatal("invariant violation: vocabulary map is nil")
	}

	if classifier.documents < 0 {
		test.Fatalf("invariant violation: document count is negative (%d)", classifier.documents)
	}
}

// TestNewClassifier_Initialization verifies map allocation and initial state setup.
func TestNewClassifier_Initialization(test *testing.T) {
	// Arrange & Act.
	classifier := NewClassifier()

	// Assert.
	verifyClassifierInvariants(test, classifier)

	if classifier.documents != 0 {
		test.Fatalf("expected 0 initial documents, got %d", classifier.documents)
	}

	if len(classifier.vocabulary) != 0 {
		test.Fatalf("expected empty initial vocabulary, got %d", len(classifier.vocabulary))
	}
}

// TestTokenize_StandardSentence verifies lowercasing and whitespace splitting.
func TestTokenize_StandardSentence(test *testing.T) {
	// Arrange.
	inputText := "Hello World  This is   Go"
	expectedTokens := []string{"hello", "world", "this", "is", "go"}

	// Act.
	producedTokens := tokenize(inputText)

	// Assert.
	if !reflect.DeepEqual(producedTokens, expectedTokens) {
		test.Fatalf("expected tokens %v, got %v", expectedTokens, producedTokens)
	}
}

// TestTokenize_EmptyString verifies empty slice return for empty text.
func TestTokenize_EmptyString(test *testing.T) {
	// Arrange.
	inputText := "   "

	// Act.
	producedTokens := tokenize(inputText)

	// Assert.
	if producedTokens != nil {
		test.Fatalf("expected nil slice for empty input, got %v", producedTokens)
	}
}

// TestTokenize_SpecialWhitespace verifies handling of tab and newline characters.
func TestTokenize_SpecialWhitespace(test *testing.T) {
	// Arrange.
	inputText := "spam\toffer\nclaim\r\nnow"
	expectedTokens := []string{"spam", "offer", "claim", "now"}

	// Act.
	producedTokens := tokenize(inputText)

	// Assert.
	if !reflect.DeepEqual(producedTokens, expectedTokens) {
		test.Fatalf("expected tokens %v, got %v", expectedTokens, producedTokens)
	}
}

// TestTrain_NilReceiver verifies graceful execution when calling Train on a nil pointer.
func TestTrain_NilReceiver(test *testing.T) {
	// Arrange.
	var classifier *Classifier

	// Act & Assert (Should not panic).
	classifier.Train("some text", "spam")
}

// TestPredict_NilReceiver verifies defensive return value when calling Predict on a nil pointer.
func TestPredict_NilReceiver(test *testing.T) {
	// Arrange.
	var classifier *Classifier

	// Act.
	predictedClass := classifier.Predict("some text")

	// Assert.
	if predictedClass != "" {
		test.Fatalf("expected empty string for nil receiver, got %q", predictedClass)
	}
}

// TestTrain_EmptyText verifies that training with whitespace-only text is ignored.
func TestTrain_EmptyText(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	// Act.
	classifier.Train("   ", "spam")

	// Assert.
	if classifier.documents != 0 {
		test.Fatalf("expected 0 documents recorded, got %d", classifier.documents)
	}
}

// TestTrain_EmptyClass verifies that training with a blank class name is ignored.
func TestTrain_EmptyClass(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	// Act.
	classifier.Train("urgent business offer", "   ")

	// Assert.
	if classifier.documents != 0 {
		test.Fatalf("expected 0 documents recorded for empty class, got %d", classifier.documents)
	}
}

// TestTrain_SingleDocument verifies state mutation after one training sample.
func TestTrain_SingleDocument(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	// Act.
	classifier.Train("hello world", "greeting")

	// Assert.
	verifyClassifierInvariants(test, classifier)

	if classifier.documents != 1 {
		test.Fatalf("expected 1 document, got %d", classifier.documents)
	}

	if classifier.classes["greeting"] != 1 {
		test.Fatalf("expected class count 1 for greeting, got %d", classifier.classes["greeting"])
	}

	if len(classifier.vocabulary) != 2 {
		test.Fatalf("expected vocabulary size 2, got %d", len(classifier.vocabulary))
	}

	if classifier.classSize["greeting"] != 2 {
		test.Fatalf("expected total words 2 for greeting, got %d", classifier.classSize["greeting"])
	}
}

// TestTrain_UninitializedStruct verifies lazy initialization of zero-value struct during Train.
func TestTrain_UninitializedStruct(test *testing.T) {
	// Arrange.
	classifier := &Classifier{}

	// Act.
	classifier.Train("valid token", "class_a")

	// Assert.
	verifyClassifierInvariants(test, classifier)

	if classifier.documents != 1 {
		test.Fatalf("expected 1 document in lazily initialized classifier, got %d", classifier.documents)
	}
}

// TestTrain_VocabularyExpansion verifies vocabulary set growth across different classes.
func TestTrain_VocabularyExpansion(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	// Act.
	classifier.Train("apple banana", "fruit")
	classifier.Train("banana cherry", "fruit")
	classifier.Train("cherry date", "other")

	// Assert.
	expectedVocabSize := 4 // apple, banana, cherry, date

	if len(classifier.vocabulary) != expectedVocabSize {
		test.Fatalf("expected vocabulary size %d, got %d", expectedVocabSize, len(classifier.vocabulary))
	}
}

// TestTrain_FeatureCountAccumulation verifies word frequency tracking per class.
func TestTrain_FeatureCountAccumulation(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	// Act.
	classifier.Train("win cash win prize", "spam")

	// Assert.
	winCount := classifier.features["spam"]["win"]
	cashCount := classifier.features["spam"]["cash"]

	if winCount != 2 {
		test.Fatalf("expected feature count 2 for 'win', got %d", winCount)
	}

	if cashCount != 1 {
		test.Fatalf("expected feature count 1 for 'cash', got %d", cashCount)
	}
}

// TestPredict_UntrainedClassifier verifies defensive empty string on untrained classifier.
func TestPredict_UntrainedClassifier(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	// Act.
	predictedClass := classifier.Predict("test input")

	// Assert.
	if predictedClass != "" {
		test.Fatalf("expected empty string for untrained classifier, got %q", predictedClass)
	}
}

// TestPredict_EmptyTextInput verifies empty string prediction when evaluation text is blank.
func TestPredict_EmptyTextInput(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()
	classifier.Train("hello world", "greeting")

	// Act.
	predictedClass := classifier.Predict("   ")

	// Assert.
	if predictedClass != "" {
		test.Fatalf("expected empty string for blank input text, got %q", predictedClass)
	}
}

// TestPredict_SpamDetection verifies binary text classification for spam messages.
func TestPredict_SpamDetection(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("free urgent offer claim discount prize", "spam")
	classifier.Train("guaranteed cash prize contact immediately", "spam")
	classifier.Train("project status meeting scheduled tomorrow", "ham")
	classifier.Train("team meeting discussion and project update", "ham")

	testSample := "claim your free cash prize"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass != "spam" {
		test.Fatalf("expected prediction 'spam', got %q", predictedClass)
	}
}

// TestPredict_HamDetection verifies binary text classification for legitimate messages.
func TestPredict_HamDetection(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("free urgent offer claim discount prize", "spam")
	classifier.Train("guaranteed cash prize contact immediately", "spam")
	classifier.Train("project status meeting scheduled tomorrow", "ham")
	classifier.Train("team meeting discussion and project update", "ham")

	testSample := "urgent project meeting scheduled tomorrow"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass != "ham" {
		test.Fatalf("expected prediction 'ham', got %q", predictedClass)
	}
}

// TestPredict_SentimentAnalysisPositive verifies sentiment classification for positive text.
func TestPredict_SentimentAnalysisPositive(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("great quality product fast shipping highly recommended", "positive")
	classifier.Train("excellent customer service solved my issue quickly", "positive")
	classifier.Train("loved the experience fantastic quality and support", "positive")

	classifier.Train("terrible experience product arrived broken and late", "negative")
	classifier.Train("poor quality worst customer service ever disappointment", "negative")
	classifier.Train("broken item useless product very disappointed overall", "negative")

	testSample := "fantastic customer service and fast delivery"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass != "positive" {
		test.Fatalf("expected prediction 'positive', got %q", predictedClass)
	}
}

// TestPredict_SentimentAnalysisNegative verifies sentiment classification for negative text.
func TestPredict_SentimentAnalysisNegative(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("great quality product fast shipping highly recommended", "positive")
	classifier.Train("excellent customer service solved my issue quickly", "positive")
	classifier.Train("terrible experience product arrived broken and late", "negative")
	classifier.Train("poor quality worst customer service ever", "negative")

	testSample := "terrible product arrived broken"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass != "negative" {
		test.Fatalf("expected prediction 'negative', got %q", predictedClass)
	}
}

// TestPredict_UnseenWordsLaplaceSmoothing verifies prediction using Laplace smoothing when tokens are missing from training vocabulary.
func TestPredict_UnseenWordsLaplaceSmoothing(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("apple pie dessert", "food")
	classifier.Train("database query index", "tech")

	testSample := "pie query index"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass != "tech" {
		test.Fatalf("expected prediction 'tech', got %q", predictedClass)
	}
}

// TestPredict_CompletelyUnseenWords verifies deterministic output when evaluating entirely new words.
func TestPredict_CompletelyUnseenWords(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("alpha beta", "class_one")
	classifier.Train("gamma delta", "class_two")

	testSample := "unknown words entirely"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass == "" {
		test.Fatalf("expected valid non-empty predicted class even for completely unseen words")
	}
}

// TestPredict_MultiClassCategorization verifies accuracy across three distinct category choices.
func TestPredict_MultiClassCategorization(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("football goal match team score player", "sports")
	classifier.Train("politics election candidate vote government policy", "politics")
	classifier.Train("cpu memory processor server cloud code software", "technology")

	testSample := "software running on server cpu"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass != "technology" {
		test.Fatalf("expected prediction 'technology', got %q", predictedClass)
	}
}

// TestPredict_CaseInsensitivity verifies prediction consistency regardless of uppercase input text.
func TestPredict_CaseInsensitivity(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("urgent offer claim prize", "spam")
	classifier.Train("project schedule meeting update", "ham")

	testSample := "URGENT CLAIM PRIZE"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass != "spam" {
		test.Fatalf("expected case-insensitive prediction 'spam', got %q", predictedClass)
	}
}

// TestPredict_SingleTokenDocument verifies prediction when evaluating a single word input.
func TestPredict_SingleTokenDocument(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("python go rust java javascript", "coding")
	classifier.Train("paris london berlin madrid rome", "travel")

	testSample := "berlin"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass != "travel" {
		test.Fatalf("expected prediction 'travel', got %q", predictedClass)
	}
}

// TestTrain_DuplicateTrainingPhrases verifies count scaling when identical documents are trained multiple times.
func TestTrain_DuplicateTrainingPhrases(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	// Act.
	classifier.Train("duplicate document text", "topic")
	classifier.Train("duplicate document text", "topic")

	// Assert.
	if classifier.documents != 2 {
		test.Fatalf("expected 2 total documents, got %d", classifier.documents)
	}

	if classifier.classes["topic"] != 2 {
		test.Fatalf("expected class count 2, got %d", classifier.classes["topic"])
	}

	if classifier.features["topic"]["duplicate"] != 2 {
		test.Fatalf("expected feature count 2 for 'duplicate', got %d", classifier.features["topic"]["duplicate"])
	}
}

// TestPredict_PriorsImpact verifies that higher class prior frequency shifts class selection when text is ambiguous.
func TestPredict_PriorsImpact(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("shared word sample", "frequent")
	classifier.Train("shared word sample", "frequent")
	classifier.Train("shared word sample", "frequent")
	classifier.Train("shared word sample", "rare")

	// Act.
	predictedClass := classifier.Predict("shared word")

	// Assert.
	if predictedClass != "frequent" {
		test.Fatalf("expected prior-dominated prediction 'frequent', got %q", predictedClass)
	}
}

// TestPredict_LongTextSample verifies log-space stability when processing long text inputs.
func TestPredict_LongTextSample(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("network router protocol packet port connection server client firewall bandwidth", "networking")
	classifier.Train("database query transaction SQL index table schema primary key join execute", "database")

	testSample := "network packet connection protocol router server port client bandwidth firewall extra words"

	// Act.
	predictedClass := classifier.Predict(testSample)

	// Assert.
	if predictedClass != "networking" {
		test.Fatalf("expected prediction 'networking', got %q", predictedClass)
	}
}

// TestTrain_SpecialPunctuationHandling verifies standard whitespace tokenization around punctuation.
func TestTrain_SpecialPunctuationHandling(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	// Act.
	classifier.Train("error! failed: connection lost.", "logs")

	// Assert.
	if !classifier.vocabulary["error!"] {
		test.Fatalf("expected literal punctuation token 'error!' in vocabulary")
	}
}

// TestPredict_TieBreakerDeterminism verifies deterministic execution when candidate classes yield scores.
func TestPredict_TieBreakerDeterminism(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	classifier.Train("common word", "class_a")
	classifier.Train("common word", "class_b")

	// Act.
	firstPrediction := classifier.Predict("common")
	secondPrediction := classifier.Predict("common")

	// Assert.
	if firstPrediction == "" {
		test.Fatalf("expected non-empty prediction on tie evaluation")
	}

	if firstPrediction != secondPrediction {
		test.Fatalf("expected deterministic tie-breaker prediction: got %q and %q", firstPrediction, secondPrediction)
	}
}

// TestTrain_RepeatedWordsInSingleDocument verifies total token count and feature counts in repetitive text.
func TestTrain_RepeatedWordsInSingleDocument(test *testing.T) {
	// Arrange.
	classifier := NewClassifier()

	// Act.
	classifier.Train("echo echo echo echo", "audio")

	// Assert.
	if classifier.classSize["audio"] != 4 {
		test.Fatalf("expected total words 4 for audio class, got %d", classifier.classSize["audio"])
	}

	if classifier.features["audio"]["echo"] != 4 {
		test.Fatalf("expected feature count 4 for 'echo', got %d", classifier.features["audio"]["echo"])
	}

	if len(classifier.vocabulary) != 1 {
		test.Fatalf("expected single unique vocabulary word, got %d", len(classifier.vocabulary))
	}
}
