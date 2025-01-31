import unittest
from process_command import process_command

class TestProcessCommand(unittest.TestCase):
    def test_forward_command(self):
        """Test that the FORWARD command returns correct motor signals."""
        self.assertEqual(process_command("FORWARD"), (1, 1))

    def test_backward_command(self):
        """Test that the BACKWARD command returns correct motor signals."""
        self.assertEqual(process_command("BACKWARD"), (-1, -1))

    def test_left_command(self):
        """Test that the LEFT command returns correct motor signals."""
        self.assertEqual(process_command("LEFT"), (-1, 1))

    def test_right_command(self):
        """Test that the RIGHT command returns correct motor signals."""
        self.assertEqual(process_command("RIGHT"), (1, -1))

    def test_stop_command(self):
        """Test that the STOP command stops both motors."""
        self.assertEqual(process_command("STOP"), (0, 0))

    def test_invalid_command(self):
        """Test that an invalid command defaults to stopping the motors."""
        self.assertEqual(process_command("JUMP"), (0, 0))
        self.assertEqual(process_command(""), (0, 0))
        self.assertEqual(process_command(None), (0, 0))

if __name__ == "__main__":
    unittest.main()
