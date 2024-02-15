package nsu.urdin;

import java.io.InputStreamReader;
import java.text.MessageFormat;
import java.util.*;
import java.util.stream.Collectors;

public class BullsAndCows {
    private static final int NUMBER_LENGTH = 6;

    public static void main(String[] args) {
        BullsAndCows gameInstance = new BullsAndCows();
        gameInstance.launch(args);
    }

    public void launch(String[] args) {
        String number;
        if (args.length > 0) {
            // only for testing purposes
            number = args[0].substring(0, NUMBER_LENGTH);
        }
        else {
            number = generateNumber(NUMBER_LENGTH);
        }

        gameLoop(number);

    }
    private String generateNumber(int length) {
        List<Integer> nums = new ArrayList<>();
        for (int i = 0; i < 10; ++i) {
            nums.add(i);
        }
        Collections.shuffle(nums);

        StringBuilder builder = new StringBuilder();
        for (int i = 0; i < length; ++i) {
            builder.append(nums.get(i));
        }

        return builder.toString();
    }

    private record MatchData(int matchesWithPosition, int matchesWithoutPosition){};
    /**
     * Count how many chars match with position and without
     * Assumes that strings contain unique letters and have equal lengths
     */
    private MatchData checkMatch(String w1, String w2) {
        int matchesWithPos = 0;
        int matchesWithoutPos = 0;
        for (int i = 0; i < w1.length(); ++i) {
            if (w1.charAt(i) == w2.charAt(i)) {
                matchesWithPos++;
            }
            else if (w2.contains(String.valueOf(w1.charAt(i)))) {
                matchesWithoutPos++;
            }
        }
        return new MatchData(matchesWithPos, matchesWithoutPos);
    }

    private boolean checkUniqueSymbols(String s) {
        return s.chars()
                .mapToObj(c -> (char)c)
                .collect(Collectors.toSet())
                .size() == s.length();
    }

    private String validateEnteredNumber(String number) {
        if (!number.chars().allMatch(Character::isDigit)) {
            return "Entered string is not a number";
        }
        if (number.length() != NUMBER_LENGTH) {
            return "Entered number has different length from generated";
        }
        if (!checkUniqueSymbols(number)) {
            return "Entered number has non unique digits";
        }
        return null;
    }


    private void gameLoop(String generatedNumber) {
        System.out.println("\t*** BULLS AND COWS ***");
        System.out.println(MessageFormat.format("Generated number of {0} digits.", NUMBER_LENGTH));
        Scanner input = new Scanner(new InputStreamReader(System.in));
        int stepsCount = 1;

        while (true) {
            System.out.println(MessageFormat.format("############ Step {0} ############", stepsCount));
            System.out.print("Enter number: ");

            String userNumber;
            if (input.hasNext()) {
                userNumber = input.next();
            } else {
                break; // for proper exiting with Ctrl+D
            }

            if (userNumber.equals(generatedNumber)) {
                System.out.println(MessageFormat.format("Congratulations! You have won in {0} step(s)", stepsCount));
                break;
            }

            String errorMessage = validateEnteredNumber(userNumber);
            if (errorMessage != null) {
                System.out.println(errorMessage);
                continue;
            }

            MatchData matchData = checkMatch(userNumber, generatedNumber);
            System.out.println(MessageFormat.format("Bulls: {0} (digits in correct position)", matchData.matchesWithPosition()));
            System.out.println(MessageFormat.format("Cows: {0} (digits in incorrect position)", matchData.matchesWithoutPosition()));
            stepsCount++;
        }

        input.close();
    }
}