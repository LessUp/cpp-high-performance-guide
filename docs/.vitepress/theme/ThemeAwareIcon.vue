<script setup lang="ts">
/**
 * ThemeAwareIcon.vue - Renders different SVG icons for light/dark modes
 *
 * Usage:
 * <ThemeAwareIcon :lightSrc="/icons/logo-light.svg" :darkSrc="/icons/logo-dark.svg" alt="Logo" />
 *
 * For simple cases, use CSS variable approach directly in SVGs with
 * the .wp-logo-primary, .wp-logo-secondary, .wp-logo-accent classes.
 */

import { useData } from 'vitepress'
import { computed } from 'vue'

interface Props {
  lightSrc: string
  darkSrc: string
  alt?: string
  width?: number
  height?: number
}

const props = withDefaults(defineProps<Props>(), {
  alt: '',
  width: 24,
  height: 24,
})

const { isDark } = useData()
const currentSrc = computed(() => isDark.value ? props.darkSrc : props.lightSrc)
</script>

<template>
  <img
    :src="currentSrc"
    :alt="alt"
    :width="width"
    :height="height"
    class="wp-theme-aware-icon"
    loading="lazy"
  />
</template>

<style scoped>
.wp-theme-aware-icon {
  display: inline-block;
  vertical-align: middle;
  transition: opacity var(--wp-transition-fast, 0.15s ease-out);
}
</style>
