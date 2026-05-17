<script setup lang="ts">
interface Props {
  title?: string
  badge?: string
  caption?: string
}

defineProps<Props>()
</script>

<template>
  <figure class="wp-diagram-canvas">
    <div v-if="title || badge" class="wp-diagram-canvas-header">
      <span v-if="title" class="wp-diagram-canvas-title">{{ title }}</span>
      <span v-if="badge" class="wp-diagram-canvas-badge">{{ badge }}</span>
    </div>
    <div class="wp-diagram-canvas-body">
      <slot />
    </div>
    <figcaption v-if="caption" class="wp-diagram-canvas-footer">
      <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
        <circle cx="12" cy="12" r="10"/>
        <line x1="12" y1="16" x2="12" y2="12"/>
        <line x1="12" y1="8" x2="12.01" y2="8"/>
      </svg>
      {{ caption }}
    </figcaption>
  </figure>
</template>
