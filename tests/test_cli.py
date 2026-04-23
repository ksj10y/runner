from __future__ import annotations

import json
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
BIN = ROOT / "bin" / "local-record-maker"


class LocalRecordMakerTests(unittest.TestCase):
    def test_generates_requested_jsonl_records(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            out = Path(td) / "records.jsonl"
            subprocess.run(
                [str(BIN), "-n", "3", "-o", str(out), "--seed", "unit"],
                check=True,
                cwd=ROOT,
            )

            lines = out.read_text(encoding="utf-8").splitlines()
            self.assertEqual(len(lines), 3)
            records = [json.loads(line) for line in lines]
            self.assertEqual([r["id"] for r in records], [1, 2, 3])
            self.assertTrue(all(r["source"] == "synthetic" for r in records))
            self.assertTrue(all("note" in r for r in records))

    def test_rejects_negative_count(self) -> None:
        with tempfile.TemporaryDirectory() as td:
            out = Path(td) / "records.jsonl"
            result = subprocess.run(
                [str(BIN), "-n", "-1", "-o", str(out)],
                cwd=ROOT,
                text=True,
                capture_output=True,
            )

            self.assertNotEqual(result.returncode, 0)
            self.assertIn("count must be", result.stderr)
            self.assertFalse(out.exists())


if __name__ == "__main__":
    unittest.main()
