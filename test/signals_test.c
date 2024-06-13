#include "testsuite.h"

describe("Awry", awry_signals)
  context("Signals")

    when("a signal is explicitly captured")
      and("the incorrect signal is captured")
        it("reports the error")
          AwryModule* subject = NULL;

          capture(2) {
            subject->signals = 0;
          }

          expect(SIGSEGV) to have been_captured
        end
      end
/*
      and("the signal is captured")
        it("runs the assertion")
          AwryModule* subject = NULL;
  
          capture(SIGSEGV) {
            subject->signals = 0;
          }
  
          expect(SIGSEGV) to have been_captured
        end
      end
    end
  */
    when("a signal is captured")
      it("reports the error")
        AwryModule* subject = NULL;
        expect(subject->signals) to equal(0)
      end
    end

    when("the signal was captured")
      it("runs the next test")
        expect(0) to equal(0)
      end
    end

  end
end