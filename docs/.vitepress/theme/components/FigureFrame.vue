<script setup lang="ts">
defineProps<{
  label?: string
  caption?: string
  note?: string
}>()
</script>

<template>
  <figure class="wp-figure">
    <figcaption v-if="label || caption" class="wp-figure__header">
      <span v-if="label" class="wp-figure__label">{{ label }}</span>
      <span v-if="caption" class="wp-figure__caption">{{ caption }}</span>
    </figcaption>
    <div class="wp-figure__body">
      <slot />
      <p v-if="note" class="wp-figure__note">{{ note }}</p>
    </div>
  </figure>
</template>
