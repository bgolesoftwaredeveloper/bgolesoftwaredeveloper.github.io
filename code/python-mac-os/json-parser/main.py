"""
main.py

Execution entry point designed to demonstrate the runtime capabilities and structural
evaluation mechanics of the custom JsonParser engine.

This module loads a highly detailed, multi-layered JSON string containing complex
pharmacological profiles (specifically targeting the beta-blocker Atenolol) and feeds
the raw buffer into the manual recursive descent parser. It prints the resultant
prettified dataset directly to standard output via the class's string formatting protocol.

Design Note:
    The variable layout uses a nested profile comprising string elements, structural
    dictionaries, linear arrays, and explicit boolean literals to comprehensively execute
    and evaluate all branches of the downstream parsing algorithm.
"""
from json_parser import JsonParser

def main():
    # Define a JSON object to work with.
    propranolol = """
    {
        "brandName": "Tenormin",
        "genericName": "Atenolol",
        "drugClass": "Beta-blocker (Selective β1)",
        "manufacturer": "Sandoz (Novartis)",
        "chemicalStructure": "C₁₄H₂₂N₂O₃",
        "mechanismOfAction": "Competitively blocks beta-1 adrenergic receptors in the heart, decreasing heart rate, cardiac output, and even blood pressure. At high doses, it may lose its selectivity and block beta-2 receptors.",
        "pharmacokinetics": {
            "absorption": "Rapidly but incompletely absorbed (~50%); peak plasma levels in 2–4 hours.",
            "distribution": "Low lipid solubility (hydrophilic); minimal distribution to the CNS (unlike Propranolol). Does not cross the blood-brain barrier effectively.",
            "metabolism": "Minimal to no hepatic metabolism. It is NOT processed by the CYP450 system.",
            "elimination": "Excreted primarily unchanged in the urine (~85-100%).",
            "halfLife": "6–7 hours"
        },
        "pharmacodynamics": [
            "Negative inotropic and chronotropic effects (slows heart and reduces squeeze).",
            "Reduction in renin secretion from the kidneys."
        ],
        "indications": [
            "Hypertension",
            "Angina Pectoris",
            "Acute Myocardial Infarction"
        ],
        "contraindications": [
            "Sinus bradycardia (Heart rate < 45 bpm).",
            "Heart block greater than first degree.",
            "Cardiogenic shock.",
            "Overt cardiac failure."
        ],
        "adverseEffects": [
            "Fatigue, dizziness, cold extremities, bradycardia, hypotension, depression (rarely)."
        ],
        "drugInteractions": [
            "Calcium Channel Blockers (e.g., Verapamil): Risk of severe bradycardia or heart block.",
            "Digoxin: Additive slowing of AV conduction.",
            "NSAIDs: May decrease the antihypertensive effect of Atenolol.",
            "Insulin: May mask the symptoms of hypoglycemia (tachycardia)."
        ],
        "dosageForms": [
            "Oral tablets (25mg, 50mg, 100mg)"
        ],
        "isControlledSubstance": false,
        "deaSchedule": "None",
        "regulatoryStatus": "FDA approved"
    }
    """

    # Instantiate the JSON parser.
    parser = JsonParser(propranolol)

    # Print a string representation of the class object.
    print(parser)

if __name__ == "__main__":
    main()