// -----------------------------------------------------------------------------
// Package: principalcomponentanalysis
// File: principal_component_analysis_test.go
//
// Description:
//
//	This file contains 30 unique unit tests for the Principal Component Analysis (PCA)
//	algorithm implementation using Gonum linear algebra primitives.
//	The test suite validates constructor initialization, defensive parameter validation,
//	mean centering, SVD factorization, sub-space dimensionality reduction, state encapsulation,
//	and terminal output matrix formatting functions.
//
//	The test suite focuses on:
//
//	  - Basic instantiation, target component configuration, and initial state invariants.
//	  - Defensive execution against nil inputs, empty matrices, insufficient samples, and component bounds.
//	  - Method execution order guards (e.g., calling Transform before Fit).
//	  - Internal mean vector arithmetic accuracy and mean centering calculations.
//	  - Subspace transformation shape correctness and output projection matrix dimensions.
//	  - State encapsulation and defensive copying of internal matrices and slices.
//	  - Formatting utility behavior with zero precision, high precision, and negative precision errors.
//
// Notes:
//   - All tests use explicit Arrange / Act / Assert structure.
//   - The testing parameter is intentionally named "test" for clarity.
//   - Descriptive variable names are used throughout.
//
// -----------------------------------------------------------------------------
package principalcomponentanalysis

import (
	"math"
	"testing"

	"gonum.org/v1/gonum/mat"
)

// verifyPCAInvariants validates model state properties and unexported invariants.
func verifyPCAInvariants(test *testing.T, pca *PrincipalComponentAnalysis) {
	test.Helper()

	if pca == nil {
		test.Fatal("invariant violation: pca instance is nil")
	}

	if pca.componentCount <= 0 {
		test.Fatalf("invariant violation: component count must be positive, got %d", pca.componentCount)
	}

	if !pca.isFitted {
		if pca.meanVector != nil {
			test.Fatal("invariant violation: meanVector should be nil prior to fitting")
		}

		if pca.projectedMatrix != nil {
			test.Fatal("invariant violation: projectedMatrix should be nil prior to fitting")
		}
	} else {
		if pca.meanVector == nil {
			test.Fatal("invariant violation: fitted model has nil meanVector")
		}

		if pca.projectedMatrix == nil {
			test.Fatal("invariant violation: fitted model has nil projectedMatrix")
		}
	}
}

// TestNewPrincipalComponentAnalysis_ValidInitialization verifies constructor setup for positive component counts.
func TestNewPrincipalComponentAnalysis_ValidInitialization(test *testing.T) {
	// Arrange.
	targetComponents := 2

	// Act.
	pca, err := NewPrincipalComponentAnalysis(targetComponents)

	// Assert.
	if err != nil {
		test.Fatalf("expected no error during initialization, got: %v", err)
	}

	verifyPCAInvariants(test, pca)

	if pca.ComponentCount() != targetComponents {
		test.Fatalf("expected component count %d, got %d", targetComponents, pca.ComponentCount())
	}

	if pca.IsFitted() {
		test.Fatal("expected newly initialized PCA model to not be fitted")
	}
}

// TestNewPrincipalComponentAnalysis_ZeroComponents verifies error return for zero component input.
func TestNewPrincipalComponentAnalysis_ZeroComponents(test *testing.T) {
	// Arrange.
	targetComponents := 0

	// Act.
	pca, err := NewPrincipalComponentAnalysis(targetComponents)

	// Assert.
	if err == nil {
		test.Fatal("expected error for zero target components, got nil")
	}

	if pca != nil {
		test.Fatalf("expected nil PCA instance on error, got %v", pca)
	}
}

// TestNewPrincipalComponentAnalysis_NegativeComponents verifies error return for negative component counts.
func TestNewPrincipalComponentAnalysis_NegativeComponents(test *testing.T) {
	// Arrange.
	targetComponents := -3

	// Act.
	pca, err := NewPrincipalComponentAnalysis(targetComponents)

	// Assert.
	if err == nil {
		test.Fatal("expected error for negative target components, got nil")
	}

	if pca != nil {
		test.Fatalf("expected nil PCA instance on error, got %v", pca)
	}
}

// TestFit_NilMatrix verifies error return when fitting against a nil matrix pointer.
func TestFit_NilMatrix(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(2)

	// Act.
	err := pca.Fit(nil)

	// Assert.
	if err == nil {
		test.Fatal("expected error when fitting nil matrix, got nil")
	}

	if pca.IsFitted() {
		test.Fatal("expected model to remain unfitted after nil matrix error")
	}
}

// TestFit_EmptyMatrix verifies error return when fitting against an empty matrix.
func TestFit_EmptyMatrix(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	emptyMatrix := new(mat.Dense)

	// Act.
	err := pca.Fit(emptyMatrix)

	// Assert.
	if err == nil {
		test.Fatal("expected error when fitting empty matrix, got nil")
	}
}

// TestFit_SingleSampleMatrix verifies error return when sample count is less than 2.
func TestFit_SingleSampleMatrix(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	singleSampleMatrix := mat.NewDense(1, 3, []float64{1.0, 2.0, 3.0})

	// Act.
	err := pca.Fit(singleSampleMatrix)

	// Assert.
	if err == nil {
		test.Fatal("expected error when fitting dataset with < 2 samples, got nil")
	}
}

// TestFit_ComponentsExceedFeatures verifies error return when target components exceeds feature count.
func TestFit_ComponentsExceedFeatures(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(5)

	sampleMatrix := mat.NewDense(4, 2, []float64{
		1.0, 2.0,
		3.0, 4.0,
		5.0, 6.0,
		7.0, 8.0,
	})

	// Act.
	err := pca.Fit(sampleMatrix)

	// Assert.
	if err == nil {
		test.Fatal("expected error when target components exceed feature count, got nil")
	}
}

// TestFit_SuccessfulFitting verifies state updates after valid fitting operation.
func TestFit_SuccessfulFitting(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(2)

	dataset := []float64{
		2.5, 2.4,
		0.5, 0.7,
		2.2, 2.9,
		1.9, 2.2,
		3.1, 3.0,
	}

	sampleMatrix := mat.NewDense(5, 2, dataset)

	// Act.
	err := pca.Fit(sampleMatrix)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful fit, got error: %v", err)
	}

	verifyPCAInvariants(test, pca)

	if !pca.IsFitted() {
		test.Fatal("expected IsFitted to return true post-fit")
	}
}

// TestComputeFeatureMeans_Accuracy verifies column arithmetic mean vector calculations.
func TestComputeFeatureMeans_Accuracy(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	dataset := []float64{
		1.0, 10.0,
		3.0, 20.0,
		5.0, 30.0,
	}

	sampleMatrix := mat.NewDense(3, 2, dataset)
	expectedMeans := []float64{3.0, 20.0}

	// Act.
	err := pca.Fit(sampleMatrix)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful fit, got: %v", err)
	}

	computedMeans := pca.FeatureMeanVector()

	for featureIndex, expectedMean := range expectedMeans {
		if math.Abs(computedMeans[featureIndex]-expectedMean) > 1e-9 {
			test.Fatalf("expected mean %.4f at index %d, got %.4f",
				expectedMean, featureIndex, computedMeans[featureIndex])
		}
	}
}

// TestTransform_UnfittedModel verifies error return when transforming before calling Fit.
func TestTransform_UnfittedModel(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	sampleMatrix := mat.NewDense(2, 2, []float64{1.0, 2.0, 3.0, 4.0})

	// Act.
	transformedMatrix, err := pca.Transform(sampleMatrix)

	// Assert.
	if err == nil {
		test.Fatal("expected error when calling Transform on unfitted model, got nil")
	}

	if transformedMatrix != nil {
		test.Fatalf("expected nil transformed matrix output, got %v", transformedMatrix)
	}
}

// TestTransform_NilMatrix verifies error return when transforming a nil matrix.
func TestTransform_NilMatrix(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	fitMatrix := mat.NewDense(2, 2, []float64{1.0, 2.0, 3.0, 4.0})

	_ = pca.Fit(fitMatrix)

	// Act.
	transformedMatrix, err := pca.Transform(nil)

	// Assert.
	if err == nil {
		test.Fatal("expected error when transforming nil matrix, got nil")
	}

	if transformedMatrix != nil {
		test.Fatalf("expected nil output on nil matrix transform, got %v", transformedMatrix)
	}
}

// TestTransform_DimensionMismatch verifies error return when transforming matrix with wrong feature count.
func TestTransform_DimensionMismatch(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	fitMatrix := mat.NewDense(3, 3, []float64{
		1.0, 2.0, 3.0,
		4.0, 5.0, 6.0,
		7.0, 8.0, 9.0,
	})

	_ = pca.Fit(fitMatrix)

	invalidMatrix := mat.NewDense(2, 2, []float64{
		1.0, 2.0,
		3.0, 4.0,
	})

	// Act.
	transformedMatrix, err := pca.Transform(invalidMatrix)

	// Assert.
	if err == nil {
		test.Fatal("expected feature dimension mismatch error, got nil")
	}

	if transformedMatrix != nil {
		test.Fatalf("expected nil transformed matrix output, got %v", transformedMatrix)
	}
}

// TestTransform_OutputShape verifies dimensions of transformed output dense matrix.
func TestTransform_OutputShape(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(2)

	fitMatrix := mat.NewDense(5, 4, []float64{
		1.0, 2.0, 3.0, 4.0,
		5.0, 6.0, 7.0, 8.0,
		9.0, 1.0, 2.0, 3.0,
		4.0, 5.0, 6.0, 7.0,
		8.0, 9.0, 1.0, 2.0,
	})

	_ = pca.Fit(fitMatrix)

	// Act.
	transformedMatrix, err := pca.Transform(fitMatrix)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful transform, got: %v", err)
	}

	transformedRows, transformedColumns := transformedMatrix.Dims()

	if transformedRows != 5 || transformedColumns != 2 {
		test.Fatalf("expected output shape (5, 2), got (%d, %d)", transformedRows, transformedColumns)
	}
}

// TestProjectionMatrix_Shape verifies shape of extracted principal component projection matrix.
func TestProjectionMatrix_Shape(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(2)

	sampleMatrix := mat.NewDense(4, 3, []float64{
		1.0, 2.0, 3.0,
		4.0, 5.0, 6.0,
		7.0, 8.0, 9.0,
		2.0, 4.0, 6.0,
	})

	_ = pca.Fit(sampleMatrix)

	// Act.
	projectionMatrix := pca.ProjectionMatrix()

	// Assert.
	if projectionMatrix == nil {
		test.Fatal("expected non-nil projection matrix")
	}

	projectionRows, projectionColumns := projectionMatrix.Dims()

	if projectionRows != 3 || projectionColumns != 2 {
		test.Fatalf("expected projection matrix dimensions (3, 2), got (%d, %d)", projectionRows, projectionColumns)
	}
}

// TestFeatureMeanVector_UnfittedModel verifies nil slice return when calling getter on unfitted model.
func TestFeatureMeanVector_UnfittedModel(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(2)

	// Act.
	means := pca.FeatureMeanVector()

	// Assert.
	if means != nil {
		test.Fatalf("expected nil mean vector for unfitted model, got %v", means)
	}
}

// TestProjectionMatrix_UnfittedModel verifies nil matrix return when calling getter on unfitted model.
func TestProjectionMatrix_UnfittedModel(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(2)

	// Act.
	projection := pca.ProjectionMatrix()

	// Assert.
	if projection != nil {
		test.Fatalf("expected nil projection matrix for unfitted model, got %v", projection)
	}
}

// TestFeatureMeanVector_DefensiveCopy verifies modifying returned mean vector slice does not mutate internal state.
func TestFeatureMeanVector_DefensiveCopy(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	sampleMatrix := mat.NewDense(2, 2, []float64{
		2.0, 4.0,
		6.0, 8.0,
	})

	_ = pca.Fit(sampleMatrix)

	// Act.
	means := pca.FeatureMeanVector()
	means[0] = 999.99

	// Assert.
	freshMeans := pca.FeatureMeanVector()

	if freshMeans[0] == 999.99 {
		test.Fatal("internal mean vector was mutated by external slice modification")
	}
}

// TestProjectionMatrix_DefensiveCopy verifies modifying returned projection matrix does not mutate internal state.
func TestProjectionMatrix_DefensiveCopy(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	sampleMatrix := mat.NewDense(2, 2, []float64{
		1.0, 3.0,
		5.0, 7.0,
	})

	_ = pca.Fit(sampleMatrix)

	// Act.
	projection := pca.ProjectionMatrix()

	originalValue := projection.At(0, 0)

	projection.Set(0, 0, 999.99)

	// Assert.
	freshProjection := pca.ProjectionMatrix()

	if freshProjection.At(0, 0) == 999.99 {
		test.Fatal("internal projection matrix was mutated by external set operation")
	}

	if freshProjection.At(0, 0) != originalValue {
		test.Fatalf("expected original value %.4f, got %.4f", originalValue, freshProjection.At(0, 0))
	}
}

// TestTransform_SingleComponentReduction verifies 1D output reduction projection.
func TestTransform_SingleComponentReduction(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	dataset := []float64{
		2.0, 4.0, 6.0,
		4.0, 8.0, 12.0,
		6.0, 12.0, 18.0,
	}

	sampleMatrix := mat.NewDense(3, 3, dataset)

	_ = pca.Fit(sampleMatrix)

	// Act.
	transformed, err := pca.Transform(sampleMatrix)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful 1D transform, got: %v", err)
	}

	rows, cols := transformed.Dims()

	if rows != 3 || cols != 1 {
		test.Fatalf("expected transformed dimensions (3, 1), got (%d, %d)", rows, cols)
	}
}

// TestTransform_NewObservations verifies transforming new un-fitted observations using learned state.
func TestTransform_NewObservations(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(2)

	trainingSet := []float64{
		1.0, 2.0, 3.0,
		4.0, 5.0, 6.0,
		7.0, 8.0, 9.0,
	}

	trainingMatrix := mat.NewDense(3, 3, trainingSet)

	_ = pca.Fit(trainingMatrix)

	newObservationMatrix := mat.NewDense(2, 3, []float64{
		2.0, 3.0, 4.0,
		5.0, 6.0, 7.0,
	})

	// Act.
	transformedNew, err := pca.Transform(newObservationMatrix)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful transform on new observations, got: %v", err)
	}

	rows, cols := transformedNew.Dims()

	if rows != 2 || cols != 2 {
		test.Fatalf("expected transformed dimensions (2, 2), got (%d, %d)", rows, cols)
	}
}

// TestFit_IdenticalFeaturesZeroVariance verifies fitting stability when all data points are identical.
func TestFit_IdenticalFeaturesZeroVariance(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	identicalDataset := []float64{
		5.0, 5.0,
		5.0, 5.0,
		5.0, 5.0,
	}

	sampleMatrix := mat.NewDense(3, 2, identicalDataset)

	// Act.
	err := pca.Fit(sampleMatrix)

	// Assert.
	if err != nil {
		test.Fatalf("expected SVD convergence on zero variance data, got: %v", err)
	}

	means := pca.FeatureMeanVector()

	if means[0] != 5.0 || means[1] != 5.0 {
		test.Fatalf("expected means [5.0, 5.0], got [%.1f, %.1f]", means[0], means[1])
	}
}

// TestFormatMatrixOutput_NilMatrix verifies error return when formatting a nil matrix pointer.
func TestFormatMatrixOutput_NilMatrix(test *testing.T) {
	// Arrange.
	var nilMatrix mat.Matrix

	// Act.
	output, err := FormatMatrixOutput(nilMatrix, 2)

	// Assert.
	if err == nil {
		test.Fatal("expected error when formatting nil matrix, got nil")
	}

	if output != "" {
		test.Fatalf("expected empty string output on error, got %q", output)
	}
}

// TestFormatMatrixOutput_NegativePrecision verifies error return when precision parameter is negative.
func TestFormatMatrixOutput_NegativePrecision(test *testing.T) {
	// Arrange.
	sampleMatrix := mat.NewDense(2, 2, []float64{1.0, 2.0, 3.0, 4.0})

	// Act.
	output, err := FormatMatrixOutput(sampleMatrix, -1)

	// Assert.
	if err == nil {
		test.Fatal("expected error for negative formatting precision, got nil")
	}

	if output != "" {
		test.Fatalf("expected empty string output on negative precision error, got %q", output)
	}
}

// TestFormatMatrixOutput_ZeroPrecision verifies string output rendering with integer rounding.
func TestFormatMatrixOutput_ZeroPrecision(test *testing.T) {
	// Arrange.
	sampleMatrix := mat.NewDense(1, 2, []float64{3.14159, 2.71828})

	expectedOutput := "\t[3, 3]\n"

	// Act.
	output, err := FormatMatrixOutput(sampleMatrix, 0)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful formatting, got error: %v", err)
	}

	if output != expectedOutput {
		test.Fatalf("expected output %q, got %q", expectedOutput, output)
	}
}

// TestFormatMatrixOutput_HighPrecision verifies string formatting at custom decimal precision.
func TestFormatMatrixOutput_HighPrecision(test *testing.T) {
	// Arrange.
	sampleMatrix := mat.NewDense(1, 1, []float64{1.234567})

	expectedOutput := "\t[1.2346]\n"

	// Act.
	output, err := FormatMatrixOutput(sampleMatrix, 4)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful formatting, got error: %v", err)
	}

	if output != expectedOutput {
		test.Fatalf("expected output %q, got %q", expectedOutput, output)
	}
}

// TestFit_FullRankComponentExtraction verifies full rank dimension reduction behavior.
func TestFit_FullRankComponentExtraction(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(3)

	sampleMatrix := mat.NewDense(4, 3, []float64{
		1.0, 0.0, 0.0,
		0.0, 2.0, 0.0,
		0.0, 0.0, 3.0,
		4.0, 4.0, 4.0,
	})

	// Act.
	err := pca.Fit(sampleMatrix)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful fit for full rank components, got: %v", err)
	}

	projection := pca.ProjectionMatrix()

	rows, cols := projection.Dims()

	if rows != 3 || cols != 3 {
		test.Fatalf("expected (3, 3) projection matrix for full rank, got (%d, %d)", rows, cols)
	}
}

// TestTransform_ZeroMeanCenteredResult verifies mean of transformed dataset approaches zero.
func TestTransform_ZeroMeanCenteredResult(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	dataset := []float64{
		10.0, 20.0,
		20.0, 40.0,
		30.0, 60.0,
	}

	sampleMatrix := mat.NewDense(3, 2, dataset)

	_ = pca.Fit(sampleMatrix)

	// Act.
	transformed, err := pca.Transform(sampleMatrix)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful transform, got: %v", err)
	}

	var componentSum float64

	rows, _ := transformed.Dims()

	for rowIndex := 0; rowIndex < rows; rowIndex++ {
		componentSum += transformed.At(rowIndex, 0)
	}

	componentMean := componentSum / float64(rows)

	if math.Abs(componentMean) > 1e-9 {
		test.Fatalf("expected mean of transformed components to be 0, got %.9f", componentMean)
	}
}

// TestFit_RefitModel verifies resetting state when calling Fit multiple times on different datasets.
func TestFit_RefitModel(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	initialMatrix := mat.NewDense(2, 2, []float64{
		1.0, 2.0,
		3.0, 4.0,
	})

	_ = pca.Fit(initialMatrix)

	secondMatrix := mat.NewDense(3, 3, []float64{
		10.0, 20.0, 30.0,
		40.0, 50.0, 60.0,
		7.0, 8.0, 9.0,
	})

	// Act.
	err := pca.Fit(secondMatrix)

	// Assert.
	if err != nil {
		test.Fatalf("expected successful re-fit, got: %v", err)
	}

	means := pca.FeatureMeanVector()

	if len(means) != 3 {
		test.Fatalf("expected 3 feature means after re-fit, got %d", len(means))
	}
}

// TestTransform_EmptyMatrixInput verifies error return when transforming an empty matrix.
func TestTransform_EmptyMatrixInput(test *testing.T) {
	// Arrange.
	pca, _ := NewPrincipalComponentAnalysis(1)

	fitMatrix := mat.NewDense(2, 2, []float64{1.0, 2.0, 3.0, 4.0})

	_ = pca.Fit(fitMatrix)

	emptyTransformMatrix := new(mat.Dense)

	// Act.
	transformedMatrix, err := pca.Transform(emptyTransformMatrix)

	// Assert.
	if err == nil {
		test.Fatal("expected error when transforming empty matrix input, got nil")
	}

	if transformedMatrix != nil {
		test.Fatalf("expected nil transformed matrix, got %v", transformedMatrix)
	}
}
