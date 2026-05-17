<script setup lang="ts">
/**
 * CodeCompare.vue - Before/After code comparison component
 *
 * Usage:
 * <CodeCompare>
 *   <template #before>```cpp\n// bad code\n```</template>
 *   <template #after>```cpp\n// good code\n```</template>
 * </CodeCompare>
 */

interface Props {
  beforeLabel?: string
  afterLabel?: string
}

const props = withDefaults(defineProps<Props>(), {
  beforeLabel: 'Before',
  afterLabel: 'After',
})
</script>

<template>
  <div class="wp-code-compare">
    <div class="wp-code-compare-panel">
      <div class="wp-code-compare-header before">
        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <circle cx="12" cy="12" r="10"/>
          <line x1="15" y1="9" x2="9" y2="15"/>
          <line x1="9" y1="9" x2="15" y2="15"/>
        </svg>
        {{ beforeLabel }}
      </div>
      <div class="wp-code-compare-content">
        <slot name="before">
          <p class="wp-code-placeholder">Add code in the <code>#before</code> slot</p>
        </slot>
      </div>
    </div>
    <div class="wp-code-compare-panel">
      <div class="wp-code-compare-header after">
        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
          <path d="M22 11.08V12a10 10 0 1 1-5.93-9.14"/>
          <polyline points="22 4 12 14.01 9 11.01"/>
        </svg>
        {{ afterLabel }}
      </div>
      <div class="wp-code-compare-content">
        <slot name="after">
          <p class="wp-code-placeholder">Add code in the <code>#after</code> slot</p>
        </slot>
      </div>
    </div>
  </div>
</template>

<style scoped>
.wp-code-compare {
  display: grid;
  grid-template-columns: 1fr 1fr;
  gap: var(--wp-space-4, 1rem);
  margin: var(--wp-space-5, 1.5rem) 0;
}

.wp-code-compare-panel {
  border: 1px solid var(--wp-line-1, #e0e0e0);
  border-radius: var(--wp-radius-2, 14px);
  overflow: hidden;
  background: var(--wp-surface-2, #f0f0f5);
}

.wp-code-compare-header {
  display: flex;
  align-items: center;
  gap: var(--wp-space-2, 0.5rem);
  padding: var(--wp-space-2, 0.5rem) var(--wp-space-4, 1rem);
  background: var(--wp-surface-meta, #e8e8f0);
  border-bottom: 1px solid var(--wp-line-1, #e0e0e0);
  font-size: 0.8rem;
  font-weight: 600;
  letter-spacing: 0.05em;
  text-transform: uppercase;
}

.wp-code-compare-header.before {
  color: oklch(0.55 0.18 25); /* Red-ish for "before" */
}

.wp-code-compare-header.after {
  color: oklch(0.55 0.18 145); /* Green for "after" */
}

/* Dark mode adjustments */
:global(.dark) .wp-code-compare-header.before {
  color: oklch(0.75 0.18 25);
}

:global(.dark) .wp-code-compare-header.after {
  color: oklch(0.75 0.18 145);
}

.wp-code-compare-header svg {
  flex-shrink: 0;
}

.wp-code-compare-content {
  padding: var(--wp-space-4, 1rem);
  overflow-x: auto;
}

.wp-code-compare-content :deep(pre) {
  margin: 0;
  padding: 0;
  background: transparent !important;
  border: none !important;
  box-shadow: none !important;
}

.wp-code-compare-content :deep(code) {
  font-family: var(--wp-font-mono, 'JetBrains Mono', monospace);
  font-size: 0.9rem;
}

.wp-code-placeholder {
  margin: 0;
  padding: var(--wp-space-4, 1rem);
  color: var(--wp-ink-3, #6a6a8a);
  font-size: 0.9rem;
  text-align: center;
  background: var(--wp-surface-1, #f8f9fa);
  border-radius: var(--wp-radius-1, 8px);
}

.wp-code-placeholder code {
  background: var(--wp-surface-meta, #e8e8f0);
  padding: 0.1rem 0.3rem;
  border-radius: 4px;
  font-size: 0.85em;
}

/* Responsive: stack on smaller screens */
@media (max-width: 960px) {
  .wp-code-compare {
    grid-template-columns: 1fr;
  }
}
</style>
