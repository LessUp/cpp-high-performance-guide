<script setup lang="ts">
import { computed } from 'vue'
import { withBase } from 'vitepress'

const props = defineProps<{
  href: string
}>()

const resolvedHref = computed(() => /^(?:[a-z]+:|\/\/|#)/i.test(props.href) ? props.href : withBase(props.href))
</script>

<template>
  <a v-bind="$attrs" :href="resolvedHref">
    <slot />
  </a>
</template>
