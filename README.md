# Phase Orbit 0.1 — Windows VST3 project

Status: funktionaler Quellcode-Prototyp. DSP-Kern unter Linux getestet. Der vollständige JUCE-Plugin-Build, Oberfläche, MIDI-Routing und Ableton-Kompatibilität sind noch NICHT verifiziert. Dieses Paket enthält keine fertige DLL/VST3-Binärdatei.

## Funktionen
- 8-stufiger Allpass-Phaser mit moduliertem Frequenzzentrum.
- Modulierter Delay-Effekt für Chorus-/Flanger-artige Klänge, 0,2–30 ms Grundverzögerung.
- Rate 0,02–12 Hz, Tiefe, bipolares Feedback, Stereo-LFO-Phasenversatz, Dry/Wet und Output.
- Phaser und Delay einzeln schaltbar, serieller Signalweg: Phaser -> Delay -> Mix/Output.
- Vier zuweisbare Makros; jedes steuert einen der acht kontinuierlichen Effektparameter. 50 % = neutral, darunter/darüber ein additiver Offset im normalisierten Parameterbereich. Der Basisregler bleibt sichtbar unverändert. Kein Mehrfachziel pro Makro in Version 0.1.
- MIDI-Learn für alle 15 Parameter: CC steuert absolut, Note-On toggelt zwischen Minimum/Maximum. Omni-Empfang auf allen MIDI-Kanälen, CC 0–127; kein Pitchbend, NRPN oder relativer Encoder-Modus. MIDI-Zuweisungen und Makroziele werden mit dem Plugin-Zustand gespeichert.
- Alle Regler, Makros und Schalter stehen dem Host als Automationsparameter zur Verfügung.
- Dunkle Oberfläche mit Pegel-/Übersteuerungsanzeige. Die Wellenlinie ist dekorativ und kein Audio-Oszilloskop.
- Mono/Stereo, Parameter-Glättung. Kein Tempo-Sync in dieser Version.

## Weg A: Auf Windows bauen
1. Visual Studio 2022 Community/Build Tools installieren; Workload **Desktopentwicklung mit C++**, einschließlich MSVC x64, Windows SDK und CMake auswählen. Git for Windows installieren.
2. ZIP vollständig entpacken. `build-windows.bat` ausführen. Internet wird zum Abrufen von JUCE 8.0.6 benötigt.
3. Nach erfolgreichem Build liegt das vollständige Plugin-Paket hier:
   `build/PhaseOrbit_artefacts/Release/VST3/Phase Orbit.vst3`
4. Gesamten Ordner `Phase Orbit.vst3` nach `C:\Program Files\Common Files\VST3` kopieren; dafür kann Windows Adminrechte anfordern. Das Skript installiert oder überschreibt selbst keine Plugins.
5. Ableton: VST3-Systemordner aktivieren und Plugins neu scannen. `Phase Orbit` auf eine Audiospur ziehen. Bei der ersten Klangprüfung mit niedrigem Abhörpegel starten: hohes Feedback kann Pegelspitzen erzeugen; dies ist kein Limiter.

## Weg B: Ohne lokale Build-Werkzeuge
Den Inhalt dieses Projektordners einschließlich `.github` in ein eigenes GitHub-Repository hochladen. `CMakeLists.txt` muss in dessen Wurzel liegen. Workflow unter Actions -> Windows VST3 -> Run workflow ausführen. Nach erfolgreichem Lauf Artifact `PhaseOrbit-Windows-x64-VST3` herunterladen und wie oben installieren. Ein GitHub-Upload/Build wurde für dieses Paket noch nicht durchgeführt. Actions-Nutzung unterliegt dem eigenen GitHub-Tarif.

## MIDI und Ableton
MIDI-Learn: Ziel im Dropdown auswählen, MIDI Learn drücken, CC-Regler bewegen oder Note senden. Erneutes Drücken bricht Learn ab. Clear MIDI entfernt ALLE MIDI-Zuweisungen.

Ableton muss MIDI an den Audioeffekt weiterleiten. Dafür eine MIDI-Spur anlegen, Controller als Eingang wählen, Monitoring auf In stellen und als MIDI-Ausgang die Audiospur mit Phase Orbit sowie das Plugin als Unterziel wählen (Bezeichnungen sind Live-versionsabhängig). Falls Live das Plugin nicht als MIDI-Ziel anbietet, kann die Integration erst nach Prüfung des Builds/der verwendeten Live-Version angepasst werden. Alternativ die Plugin-Parameter über Lives Configure-Funktion verfügbar machen und Lives eigene MIDI-Zuweisung verwenden.

Makros lassen sich ebenso über Lives Automation oder Rack-Makros steuern. Plugin-MIDI-Learn und Lives MIDI-Mapping sind zwei getrennte Möglichkeiten. Die direkte VST3-Parameterzuweisung im Audiothread muss mit dem konkreten Host validiert werden.

## Tastatur
Nach einem Klick auf den freien Hintergrund der Oberfläche:
- Q/W/E/R erhöht Makro 1/2/3/4 um 2 Prozentpunkte.
- A/S/X/F senkt Makro 1/2/3/4 um 2 Prozentpunkte.
- P toggelt Phaser, D toggelt Delay, B toggelt Bypass.

Die Kürzel sind fest und gelten nur bei Fokus auf der Plugin-Oberfläche. Host und fokussierte Text-/Reglerfelder können Eingaben abfangen. Keine globalen Windows-Hotkeys. Für Steuerung bei geschlossener Oberfläche Lives Key-Mapping verwenden.

## Teststatus und Grenzen
`tests/engine_test.cpp`: Stille, extreme Parameteränderungen einschließlich Feedback ±0,9, Mono/Stereo und 22,05/44,1/48/96/192 kHz, Dry-Mix und Bypass geprüft. Kein vollständiger VST3-Build oder Host-Test durchgeführt. Kein behaupteter Hörtest. Vor Produktiveinsatz sind Windows-Compile, Plugin-Validator, GUI-Prüfung sowie Tests von Automation, Save/Reload und MIDI in Live erforderlich. Momentan keine Werks-Presets, kein Installer, kein Signieren.

## Abhängigkeiten
JUCE 8.0.6 wird per CMake FetchContent geladen, nicht mitgeliefert. JUCE-Lizenz und VST3-Bedingungen vor Weitergabe/Veröffentlichung prüfen. Referenz: https://github.com/juce-framework/JUCE/tree/8.0.6 ; CMake-Dokumentation: https://github.com/juce-framework/JUCE/blob/8.0.6/docs/CMake%20API.md . Projektlizenz: AGPL-3.0-only (siehe LICENSE). JUCE wird unter der AGPLv3 genutzt. Abhängigkeiten behalten ihre jeweiligen Lizenzen. Bei Weitergabe von Binärdateien den vollständigen zugehörigen Quellcode dieses Versionsstands und die Lizenztexte mitliefern.
