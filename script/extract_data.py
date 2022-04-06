from fractions import Fraction
import note_seq as ns
import numpy as np
import math
import sys

def float_to_fraction(num):
    if num == 0.33:
        return Fraction(1, 3)
    else:
        return Fraction(num)

if __name__ == "__main__":
    filepath = sys.argv[1]
    sequence = ns.midi_file_to_note_sequence(filepath)
    pitches = [note.pitch for note in sequence.notes]
    print(f"pitches: {pitches}")
    note_values = [note.end_time-note.start_time for note in sequence.notes]
    note_values.append(0.3289583333333334)
    print(f"note_values: {note_values}")
    rounded_note_values = [round(note, 2) for note in note_values]
    print(f"rounded_note_values: {rounded_note_values}")
    unique_rounded_note_values = np.unique(rounded_note_values)
    print(f"unique_rounded_note_values: {unique_rounded_note_values}")
    fraction_unique_rounded_note_values = [float_to_fraction(note) for note in unique_rounded_note_values]
    print(f"fraction_unique_rounded_note_values: {fraction_unique_rounded_note_values}")
    fraction_rounded_note_values = [float_to_fraction(note) for note in rounded_note_values]
    print(f"fraction_rounded_note_values: {fraction_rounded_note_values}")
    lcm = math.lcm(*[fraction.denominator for fraction in fraction_unique_rounded_note_values])
    print(f"lcm: {lcm}")
    output = []
    for i in range(0, len(pitches)):
        multiplier = int(lcm / fraction_rounded_note_values[i].denominator)
        for j in range (0, multiplier):
            output.append(pitches[i])
    print(f"output: {output}")
