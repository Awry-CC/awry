#include "testsuite.h"

void mockSuite() {}

describe("Awry Core", awry_core)

  AwryModule subject = {
    .assertions = 0,
    .test_cases = 0,
    .passes = 0,
    .failures = 0,
    .register_suite = __awry->register_suite,
    .register_block = __awry->register_block,
    .step_back      = __awry->step_back,
    .clear          = __awry->clear
  };

  context("core")
    context(".register_suite")
      subject.register_suite(&subject, "MockSuite", mockSuite);

      it("adds the suite to the suites list")
        expect(subject.current->name) to equal("MockSuite")
      end

      it("sets a root block as the current block")
        expect(subject.current->current_block->block_type) to equal(AWRY_ROOT_TYPE)
      end
    end

    context(".register_block")
      when("the block is not an IT clause")
        subject.register_block(AWRY_CONTEXT_TYPE, &subject, "Mock Context");

        it("adds the block to the children")
          expect(subject.current->current_block->previous->children.used) to equal(1)
        end

        it("sets the current block to the newly registered block")
          expect(subject.current->current_block->name) to equal("Mock Context")
        end
      end

      when("the block is an IT clause")
        subject.register_block(AWRY_IT_TYPE, &subject, "Mock It Clause");

        it("adds the block to the it_blocks")
          expect(subject.current->current_block->it_blocks.used) to equal(1)
        end

        it("sets the it_flag")
          expect(subject.current->it_flag) to equal(1)
        end
      end
    end

    context(".step_back")
      when("the block is not an IT clause")
        subject.register_block(AWRY_CONTEXT_TYPE, &subject, "Mock Context 2");
        subject.current->it_flag = 0;

        it("steps back to the previous block")
          void* previous = subject.current->current_block;
          subject.step_back(&subject);
          void* current = subject.current->current_block;

          expect(previous) to not equal(current)
        end
      end

      when("the block is an IT clause")
        subject.register_block(AWRY_IT_TYPE, &subject, "Mock It Clause");

        it("does not step back from the current block")
          void* previous = subject.current->current_block;
          subject.step_back(&subject);
          void* current = subject.current->current_block;

          expect(previous) to equal(current)
        end
      end    
    end

  end
end