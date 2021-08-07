using System;
using System.Speech.Recognition;
using System.Speech.Synthesis;

public class Program {
	static void Speak(string text) {
		SpeechSynthesizer synthesizer = new SpeechSynthesizer();
		synthesizer.Speak(text);
	}

	static void Recognize() {
		SpeechRecognitionEngine recognizer = new SpeechRecognitionEngine();//new System.Globalization.CultureInfo("en-US"));
		recognizer.SetInputToDefaultAudioDevice();
		recognizer.LoadGrammar(new DictationGrammar());
		recognizer.SpeechRecognized += new EventHandler<SpeechRecognizedEventArgs>(OnSpeechRecognized);
		recognizer.RecognizeAsync(RecognizeMode.Multiple);
	}

	static void OnSpeechRecognized(object sender, SpeechRecognizedEventArgs e) {
		if (e.Result != null)  
		{
			//System.Console.WriteLine("Hi");
			System.Console.WriteLine(e.Result.Text);  
			Speak(e.Result.Text);
		}
	}

	public static void Main() {
		//Speak("пиздец блядь сука");
		Recognize();

		for(;;)
			System.Threading.Thread.Sleep(30);

		System.Console.WriteLine("Hi");
	}
}